/** @file game.c
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief A 1v1 tetris game
 */

#include <stdbool.h>

#include "piece.h"
#include "board.h"
#include "packet.h"

// API
#include "system.h"
#include "button.h"
#include "led.h"
#include "task.h"
#include "tinygl.h"
#include "navswitch.h"
#include "font5x7_1.h"
#include "ir_uart.h"

// Task frequency (in Hz)
#define BUTTON_TASK_FREQ 100
#define DISPLAY_TASK_FREQ 300
#define IR_TASK_FREQ 100
#define BOARD_MOVE_DOWN_FREQ 1 /* 1s */

static bool game_over = false;
static void handle_packet(packet_t packet) 
{
    switch (packet.id)
    {
    case PAIRING_PACKET:
        {
            // recvd pairing packet, set the rng_seed and respond
            // uint8_t rng_seed = packet.data;

            packet_t ack = {
                .id = PAIRING_ACK_PACKET,
                .data = 0
            };

            packet_send(ack);
            // TODO: start 3 2 1 countdown
            break;
        }

    case PAIRING_ACK_PACKET:
        {
            // recvd pairing ack. maybe have a check to confirm we actuall sent a pairing packet
            // otherwise start 3 2 1 countdown
            break;
        }

    case PING_PACKET:
        {
            // todo
            // maybe we agree that whoever sent the Pairing packet should be the one sending the Ping packet
            // the other board only sends pong
            // that way each board knows which packet to expect and recv
            break;
        }

    case PONG_PACKET:
        {
            // todo
            break;
        }
    
    case LINE_CLEAR_PACKET:
        {
            // TODO
            break;
        }

    case DIE_PACKET:
        {
            // TODO
            break;
        }

    case DIE_ACK_PACKET:
        {
            // TODO
            break;
        }

    default:
        break;
    }
}

static void button_task_init(void)
{
    button_init();
}

static void button_task(__unused__ void *data)
{
    // Button 1
    {
        button_update();
        
        // TODO: Testing code, remove later
        if (button_push_event_p(BUTTON1))
            piece_rotate(current_piece);

        if (button_push_event_p(BUTTON1))
        {
            packet_t packet = {
                .id = PING_PACKET,
                .data = 'B' - 'A'
            };
            handle_packet(packet);
        }
        else if (button_release_event_p(BUTTON1))
        {
            packet_t packet = {
                .id = PONG_PACKET,
                .data = 'B' - 'A'
            };
            handle_packet(packet);
        }
    }

    // Nav Switch
    {
        navswitch_update ();

        // TODO: Testing code, remove later
        if (navswitch_push_event_p(NAVSWITCH_PUSH))
        {
            board_place_piece(board, current_piece);
            piece_generate_next();
        }

        // TODO: Remove after testing
        if (navswitch_push_event_p(NAVSWITCH_NORTH))
            piece_move(current_piece, DIRECTION_UP);

        // Move current piece
        if (navswitch_push_event_p(NAVSWITCH_EAST))
            piece_move(current_piece, DIRECTION_RIGHT);

        if (navswitch_push_event_p(NAVSWITCH_WEST))
            piece_move(current_piece, DIRECTION_LEFT);

        if (navswitch_push_event_p(NAVSWITCH_SOUTH))
            piece_move(current_piece, DIRECTION_DOWN);
    }
}

static void display_task_init(void)
{
    tinygl_init(DISPLAY_TASK_FREQ);
    tinygl_font_set(&font5x7_1);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_NORMAL);
    // tinygl_text("Hello, World!");
}

void screen_clear(void)
{
    for (int x = 0; x < TINYGL_WIDTH; x++)
    {
        for (int y = 0; y < TINYGL_HEIGHT; y++)
        {
            tinygl_point_t point = { x, y };
            tinygl_draw_point(point, 0);
        }
    }
}

static void game_over_screen(__unused__ void *data)
{
    static bool game_over_init = false;
    if (!game_over_init)
    {
        screen_clear();
        tinygl_text("Game Over");
        game_over_init = true;
    }

    tinygl_update();
}

static void display_task(__unused__ void *data)
{
    // TODO: Use an enum for the current game state
    if (game_over)
    {
        game_over_screen(data);
        return;
    }

    // Clear screen
    screen_clear();

    // draw placed board points
    for (int x = 0; x < TINYGL_WIDTH; x++)
    {
        for (int y = 0; y < TINYGL_HEIGHT; y++)
        {
            if (board->tiles[x][y])
            {
                tinygl_point_t point = { x, y };
                tinygl_draw_point(point, 1);
            }
        }
    }

    piece_draw(current_piece);
    tinygl_update();
}

static void board_move_down_task(__unused__ void *data)
{
    if (game_over)
        return;

    // Detect when a new piece has been spawned, skip moving for this iteration
    // so the piece isn't moved down immediately as soon as it's spawnedk
    static piece_t* old_piece = 0;
    if (old_piece != current_piece)
    {
        old_piece = current_piece;
        return;
    }

    bool was_moved = piece_move(current_piece, DIRECTION_DOWN);

    // piece was not able to be moved down, so we place this piece on the board at the current location
    // and spawn the next piece
    if (!was_moved)
    {
        board_place_piece(board, current_piece);
        bool valid_pos = piece_generate_next();
        if (!valid_pos)
            game_over = true;
    }
}

static void ir_update_task(__unused__ void *data)
{
    packet_t packet;
    bool recvd_packet = packet_get(&packet);
    if (!recvd_packet)
        return;
    
    handle_packet(packet);
}

int main(void)
{
    system_init();
    ir_uart_init();
    led_init();

    board_init();
    piece_generate_next();

    // Task init
    display_task_init();
    button_task_init();

    // Run tasks
    task_t tasks[] =
        {
            {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_FREQ},
            {.func = button_task, .period = TASK_RATE / BUTTON_TASK_FREQ},
            {.func = board_move_down_task, .period = TASK_RATE /  BOARD_MOVE_DOWN_FREQ},
            {.func = ir_update_task, .period = TASK_RATE /  IR_TASK_FREQ}
        };

    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
