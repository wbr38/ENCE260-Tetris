#include <stdbool.h>

#include "block.h"
#include "grid.h"

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
#define GRID_MOVE_DOWN_FREQ 1 /* 1s */

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
            block_rotate(current_block);

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
            grid_place_block(grid, current_block);
            block_generate_next();
        }

        // TODO: Remove after testing
        if (navswitch_push_event_p(NAVSWITCH_NORTH))
            block_move(current_block, DIRECTION_UP);

        // Move current block
        if (navswitch_push_event_p(NAVSWITCH_EAST))
            block_move(current_block, DIRECTION_RIGHT);

        if (navswitch_push_event_p(NAVSWITCH_WEST))
            block_move(current_block, DIRECTION_LEFT);

        if (navswitch_push_event_p(NAVSWITCH_SOUTH))
            block_move(current_block, DIRECTION_DOWN);
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

    // draw placed grid points
    for (int x = 0; x < TINYGL_WIDTH; x++)
    {
        for (int y = 0; y < TINYGL_HEIGHT; y++)
        {
            if (grid->tiles[x][y])
            {
                tinygl_point_t point = { x, y };
                tinygl_draw_point(point, 1);
            }
        }
    }

    block_draw(current_block);
    tinygl_update();
}

static void grid_move_down_task(__unused__ void *data)
{
    if (game_over)
        return;

    bool place_block = block_move(current_block, DIRECTION_DOWN);
    if (!place_block) {
        grid_place_block(grid, current_block);
        bool valid_pos = block_generate_next();
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

    grid_init();
    block_generate_next();

    // Task init
    display_task_init();
    button_task_init();

    // Run tasks
    task_t tasks[] =
        {
            {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_FREQ},
            {.func = button_task, .period = TASK_RATE / BUTTON_TASK_FREQ},
            {.func = grid_move_down_task, .period = TASK_RATE /  GRID_MOVE_DOWN_FREQ},
            // {.func = ir_update_task, .period = TASK_RATE /  IR_TASK_FREQ}
        };

    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
