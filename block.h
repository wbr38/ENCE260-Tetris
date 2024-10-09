#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include "tinygl.h"

#define BLOCK_NUM_ROTATIONS 4 // each block has precalculated 4 rotations
#define BLOCK_NUM_POINTS    4 // each block is defined with 4 pixel points

typedef enum
{
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction_t;

// Represents a tetris block
typedef struct
{
    tinygl_point_t points[BLOCK_NUM_ROTATIONS][BLOCK_NUM_POINTS];
    tinygl_point_t pos;
    direction_t direction;
} block_t;

extern block_t* current_block;

bool block_generate_next(void);
tinygl_point_t* block_get_points(block_t* block, direction_t direction);

bool block_move(block_t *block, direction_t direction);
void block_rotate(block_t *block);
void block_draw(block_t *block);

#endif // BLOCK_H