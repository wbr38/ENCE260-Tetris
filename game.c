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

// Task frequency (in Hz)
#define BUTTON_TASK_FREQ 100
#define DISPLAY_TASK_FREQ 300
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
            grid_t* grid = grid_get();
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

static void display_task(__unused__ void *data)
{
    tinygl_update();

    // Clear screen
    for (int x = 0; x < TINYGL_WIDTH; x++)
    {
        for (int y = 0; y < TINYGL_HEIGHT; y++)
        {
            tinygl_point_t point = { x, y };
            tinygl_draw_point(point, 0);
        }
    }

    if (game_over)
        return;

    // draw placed grid points
    grid_t* grid = grid_get();
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
}

static void grid_move_down_task(__unused__ void *data)
{
    if (game_over)
        return;

    grid_t* grid = grid_get();
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

int main(void)
{
    system_init();

    grid_init();
    block_generate_next();

    led_init();

    // Task init
    display_task_init();
    button_task_init();

    // Run tasks
    task_t tasks[] =
        {
            {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_FREQ},
            {.func = button_task, .period = TASK_RATE / BUTTON_TASK_FREQ},
            {.func = grid_move_down_task, .period = TASK_RATE /  GRID_MOVE_DOWN_FREQ}
        };

    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
