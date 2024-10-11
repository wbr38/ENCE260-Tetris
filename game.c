#include <stdbool.h>

#include "piece.h"
#include "board.h"

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
#define IR_TASK_FREQ 300
#define BOARD_MOVE_DOWN_FREQ 1 /* 1s */

static bool game_over = false;

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

        if (button_down_p(BUTTON1))
            led_set(LED1, 1);
        else
            led_set(LED1, 0);
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

    bool place_piece = piece_move(current_piece, DIRECTION_DOWN);
    if (!place_piece) {
        board_place_piece(board, current_piece);
        bool valid_pos = piece_generate_next();
        if (!valid_pos)
        {
            game_over = true;
        }
    }
}

static char character = 'A';
static void ir_update_task(__unused__ void *data)
{
    tinygl_update ();

    navswitch_update ();
    
    if (navswitch_push_event_p (NAVSWITCH_NORTH))
    {
        character++;
        ir_uart_putc(character);
    }

    if (navswitch_push_event_p (NAVSWITCH_SOUTH))
    {
        character--;
        ir_uart_putc(character);
    }

    // if (navswitch_push_event_p (NAVSWITCH_PUSH))
    //     ir_uart_putc(character);

    if (ir_uart_read_ready_p ())
    {
        char c = ir_uart_getc ();
        if (c >= 'A' && c <= 'Z')
            character = c;
    }

    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
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
            // {.func = ir_update_task, .period = TASK_RATE /  IR_TASK_FREQ}
        };

    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
