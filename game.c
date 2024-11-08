/** @file game.c
 *  @authors William Brown (wbr38), Matthew Wills (mwi158)
 *  @date 12 October 2024
 *  @brief A multiplayer 1v1 tetris game
 */

#include <stdbool.h>

#include "board.h"
#include "game_data.h"
#include "packet.h"
#include "piece.h"

// API headers
#include <button.h>
#include <font5x7_1.h>
#include <ir_uart.h>
#include <led.h>
#include <navswitch.h>
#include <system.h>
#include <task.h>
#include <tinygl.h>

// Task frequency (in Hz)
#define BUTTON_TASK_FREQ      100  // 1/10  -> 10ms
#define DISPLAY_TASK_FREQ     300  // 1/300 -> 3.33ms
#define IR_TASK_FREQ          100  // 1/100 -> 10ms
#define LED_FLASH_TASK_FREQ   8    // 1/8   -> 125ms
#define BOARD_MOVE_DOWN_FREQ  1    // 1/1   -> 1s
#define SEND_PACKET_TASK_FREQ 2    // 1/2   -> 500ms

// Constants
#define TINYGL_SPEED 25

/**
 * Task to poll and handle the push button and nav switch controls
 */
static void button_task(__unused__ void* data)
{
    button_update();
    navswitch_update();

    switch (game_data->game_state)
    {
    case GAME_STATE_MAIN_MENU:
        {
            // Send pairing packet on nav push
            // the other board should respond with PairingAck, then the game will commence.
            if (navswitch_push_event_p(NAVSWITCH_PUSH))
            {
                packet_t pairing_packet = {
                    .id = PAIRING_PACKET,
                    .data = game_data->rng_seed,
                };
                packet_send(pairing_packet);
                game_data->host = true;
            }
            return;
        }

    case GAME_STATE_PLAYING:
        {
            // Rotate piece
            if (navswitch_push_event_p(NAVSWITCH_PUSH))
                piece_rotate(game_data->current_piece);

            // Move current piece
            if (navswitch_push_event_p(NAVSWITCH_EAST))
                piece_move(game_data->current_piece, DIRECTION_RIGHT);

            if (navswitch_push_event_p(NAVSWITCH_WEST))
                piece_move(game_data->current_piece, DIRECTION_LEFT);

            if (navswitch_push_event_p(NAVSWITCH_SOUTH))
                piece_move(game_data->current_piece, DIRECTION_DOWN);

            return;
        }

    case GAME_STATE_GAME_OVER:
        {
            // Restart game, reinitialise data
            if (navswitch_push_event_p(NAVSWITCH_PUSH))
                game_data_init();
        }

    default:
        break;
    }
}

/**
 * Task to update the LED matrix display.
 */
static void display_task(__unused__ void* data)
{
    // Detetct when game_state has been changed.
    // The calls to tinygl_text should only be done once when the state has changed.
    static game_state_t old_state = 42;
    bool state_changed = game_data->game_state != old_state;
    old_state = game_data->game_state;

    switch (game_data->game_state)
    {
    case GAME_STATE_MAIN_MENU:
        {
            if (state_changed)
                tinygl_text(" Tetris");

            break;
        }

    case GAME_STATE_STARTING:
        {
            // Display a 3 2 1 countdown before starting the game
            // This task runs at a frequency of DISPLAY_TASK_FREQ. Once ticks == DISPLAY_TASK_FREQ, one second has elapsed.

            static uint16_t ticks = 0;
            if (ticks == 0)
            {
                tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
                tinygl_text("3");
            }
            else if (ticks == DISPLAY_TASK_FREQ)
                tinygl_text("2");
            else if (ticks == DISPLAY_TASK_FREQ * 2)
                tinygl_text("1");
            else if (ticks == DISPLAY_TASK_FREQ * 3)
            {
                game_data->game_state = GAME_STATE_PLAYING;
                ticks = 0;
                tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
                return;  // don't increase ticks below, since we want to reset here
            }

            ticks++;
            break;
        }

    case GAME_STATE_PLAYING:
        {
            tinygl_clear();

            // draw placed board points
            for (int8_t x = 0; x < TINYGL_WIDTH; x++)
            {
                for (int8_t y = 0; y < TINYGL_HEIGHT; y++)
                {
                    if (game_data->board->tiles[x][y])
                    {
                        tinygl_point_t point = {x, y};
                        tinygl_draw_point(point, 1);
                    }
                }
            }

            piece_draw(game_data->current_piece);
            break;
        }

    case GAME_STATE_DEAD:
        {
            if (state_changed)
            {
                tinygl_clear();
                tinygl_text(" DEAD");
            }

            break;
        }

    case GAME_STATE_GAME_OVER:
        {
            if (state_changed)
            {
                tinygl_clear();

                if (game_data->our_lines_cleared > game_data->their_lines_cleared)
                    tinygl_text(" WIN");
                else if (game_data->our_lines_cleared < game_data->their_lines_cleared)
                    tinygl_text(" LOSE");
                else
                    tinygl_text(" DRAW");
            }
            break;
        }

    case GAME_STATE_PAUSED:
        {
            tinygl_clear();
            for (uint8_t y = 1; y < TINYGL_HEIGHT - 1; y++)
            {
                tinygl_point_t col2 = {1, y};
                tinygl_point_t col4 = {3, y};
                tinygl_pixel_set(col2, 1);
                tinygl_pixel_set(col4, 1);
            }

            break;
        }

    default:
        break;
    }

    tinygl_update();
}

/**
 * Used to move the current piece down automatically. Will place the piece on
 * the board if there is nowhere to move down, and will spawn the next piece to be placed.
 */
static void board_move_down_task(__unused__ void* data)
{
    if (game_data->game_state != GAME_STATE_PLAYING)
        return;

    // Detect when a new piece has been spawned, skip moving for this iteration
    // so the piece isn't moved down immediately as soon as it's spawned
    static piece_t* old_piece = 0;
    if (old_piece != game_data->current_piece)
    {
        old_piece = game_data->current_piece;
        return;
    }

    bool was_moved = piece_move(game_data->current_piece, DIRECTION_DOWN);

    // the piece was not able to be moved down, so place the piece on the board at the currenet location
    if (!was_moved)
    {
        board_place_piece(game_data->board, game_data->current_piece);
        bool valid_pos = piece_generate_next(&game_data->current_piece);

        // next piece was not able to be spawned, so we have died.
        if (!valid_pos)
            game_data->game_state = GAME_STATE_DEAD;
    }
}

/**
 * Task to handle any IR packets that have been received.
 */
static void ir_update_task(__unused__ void* data)
{
    packet_t packet;
    bool recvd_packet = packet_get(&packet);
    if (!recvd_packet)
        return;

    handle_packet(packet);
}

/**
 * Task used to flash the blue LED when the other board has cleared a number of lines.
 */
static void led_flash_task(__unused__ void* data)
{
    // each alternating call of this task just sets the LED to false and does nothing else
    // this gives a flashing effect
    static bool toggle = false;
    toggle = !toggle;
    if (toggle)
    {
        led_set(LED1, false);
        return;
    }

    // Check if the other player has cleared more lines since the last time this task ran
    static uint8_t num_flashed = 0;
    if (num_flashed < game_data->their_lines_cleared)
    {
        led_set(LED1, true);
        num_flashed++;
    }
}

/**
 * Task used to periodically send packets to the other board.
 */
static void send_packet_task(__unused__ void* data)
{
    // Send die packet, and check if the game is over
    check_die_packet();
    game_data_check_game_over();

    // Ping / Pong functionality
    check_ping_pong_packet();
    game_data_check_pause();
}

/**
 * Initialise the ucfk4 system and components.
 */
static inline void environment_init(void)
{
    // misc api
    system_init();
    ir_uart_init();
    timer_init();
    button_init();

    led_init();
    led_set(LED1, false);

    // display
    tinygl_init(DISPLAY_TASK_FREQ);
    tinygl_font_set(&font5x7_1);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(TINYGL_SPEED);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_NORMAL);
}

int main(void)
{
    environment_init();
    game_data_init();

    // Run tasks
    task_t tasks[] =
        {
            {.func = display_task,         .period = TASK_RATE / DISPLAY_TASK_FREQ    },
            {.func = button_task,          .period = TASK_RATE / BUTTON_TASK_FREQ     },
            {.func = board_move_down_task, .period = TASK_RATE / BOARD_MOVE_DOWN_FREQ },
            {.func = ir_update_task,       .period = TASK_RATE / IR_TASK_FREQ         },
            {.func = led_flash_task,       .period = TASK_RATE / LED_FLASH_TASK_FREQ  },
            {.func = send_packet_task,     .period = TASK_RATE / SEND_PACKET_TASK_FREQ}
    };

    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
