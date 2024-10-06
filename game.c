#include <stdbool.h>

#include "system.h"
#include "button.h"
#include "led.h"
#include "task.h"
#include "tinygl.h"
#include "font5x7_1.h"

// Task frequency (in Hz)
#define BUTTON_TASK_RATE 100
#define DISPLAY_TASK_RATE 250
#define TIMER_TASK_RATE 1 /* 1s */

static void button_task_init(void)
{
    button_init();
}

static void button_task(__unused__ void *data)
{
    button_update();

    bool button_down = button_down_p(BUTTON1);
    if (button_down)
        led_set(LED1, 1);
    else
        led_set(LED1, 0);
}

static void display_task_init(void)
{
    tinygl_init(DISPLAY_TASK_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_NORMAL);
    tinygl_text("Hello, World!");
}

static void display_task(__unused__ void *data)
{
    tinygl_update();
}

int main(void)
{
    system_init();

    led_init();

    // Task init
    display_task_init();
    button_task_init();

    // Run tasks
    task_t tasks[] =
        {
            {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
            {.func = button_task, .period = TASK_RATE / BUTTON_TASK_RATE},
        };

    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
