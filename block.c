#include "block.h"
#include "grid.h"

#include <string.h>
#include <stdlib.h>

#include "navswitch.h"

#define BLOCK_COUNT 7  // 7 total tetris blocks

block_t* current_block = 0; // The current block being placed

// All tetris blocks and their rotations
// Positions are relative, centered around a point (see image: https://cdn.harddrop.com/1/17/SRS-true-rotations.png)
// Left=Negative, Right=Positive, Up=Negative, Down=Positive
const tinygl_point_t blocks[BLOCK_COUNT][BLOCK_NUM_ROTATIONS][BLOCK_NUM_POINTS] = {

    // I Block
    {
        { {-1, 0}, {0, 0}, {1, 0}, {2, 0} },
        { {0, -1}, {0, 0}, {0, 1}, {0, 2} },
        { {-2, 0}, {-1, 0}, {0, 0}, {1, 0} },
        { {0, -2}, {0, -1}, {0, 0}, {0, 1} },
    },

    // J Block
    {
        { {-1, -1}, {-1, 0}, {0, 0}, {1, 0} },
        { {1, -1}, {0, -1}, {0, 0}, {0, 1} },
        { {-1, 0}, {0, 0}, {1, 0}, {1, 1} },
        { {-1, 1}, {0, 1}, {0, 0}, {0, -1} },
    },

    // L Block
    {
        { {-1, 0}, {0, 0}, {1, 0}, {1, -1} },
        { {0, 1}, {0, 0}, {0, -1}, {1, 1} },
        { {-1, 1}, {-1, 0}, {0, 0}, {1, 0} },
        { {-1, -1}, {0, -1}, {0, 0}, {0, 1} },
    },

    // O (Square) Block
    {
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
    },

    // S Block
    {
        { {-1, 0}, {0, 0}, {0, -1}, {1, -1} },
        { {0, -1}, {0, 0}, {1, 0}, {1, 1} },
        { {-1, 1}, {0, 1}, {0, 0}, {1, 0} },
        { {-1, 0}, {-1, -1}, {0, 0}, {0, 1} },
    },

    // T Block
    {
        { {-1, 0}, {0, 0}, {0, -1}, {1, 0} },
        { {0, -1}, {0, 0}, {0, 1}, {1, 0} },
        { {-1, 0}, {0, 0}, {1, 0}, {0, 1} },
        { {-1, 0}, {0, 0}, {0, -1}, {0, 1} },
    },

    // Z Block
    {
        { {-1, -1}, {0, -1}, {0, 0}, {1, 0} },
        { {0, 1}, {0, 0}, {1, 0}, {1, -1} },
        { {-1, 0}, {0, 0}, {0, 1}, {1, 1} },
        { {-1, 0}, {-1, 1}, {0, 0}, {0, -1} },
    },
};

bool block_generate_next()
{
    static uint8_t _nextBlockId = 0;

    if (current_block != NULL)
        free(current_block);

    block_t* block = malloc(sizeof(block_t));
    memcpy(block->points, blocks[_nextBlockId], sizeof(block->points));
    block->direction = DIRECTION_UP;

    block->pos = (tinygl_point_t){
        .x = TINYGL_WIDTH / 2, // spawn block initially in center
        .y = 1
    };

    current_block = block;

    _nextBlockId++;
    if (_nextBlockId >= ARRAY_SIZE(blocks))
        _nextBlockId = 0;

    // check if current_block pos is valid
    bool valid_pos = grid_valid_position(grid, current_block, current_block->pos.x, current_block->pos.y, current_block->direction);
    return valid_pos;
}

tinygl_point_t* block_get_points(block_t* block, direction_t direction)
{
    return block->points[direction];
}

void block_rotate(block_t *block)
{
    direction_t new_dir = (block->direction + 1) % BLOCK_NUM_ROTATIONS;

    bool is_valid = grid_valid_position(grid, block, block->pos.x, block->pos.y, new_dir);
    if (!is_valid)
        return;

    block->direction = new_dir;
}

bool block_move(block_t *block, direction_t direction)
{
    int x = block->pos.x;
    int y = block->pos.y;

    switch (direction)
    {
    case DIRECTION_RIGHT:
        x++;
        break;

    case DIRECTION_LEFT:
        x--;
        break;

    case DIRECTION_DOWN:
        y++;
        break;

    // TODO: Remove after testing
    case DIRECTION_UP:
        y--;
        break;

    default:
        break;
    }

    // TODO: Check new position is valid. is_valid_pos(points, x, y, block->direction)
    
    bool is_valid = grid_valid_position(grid, block, x, y, block->direction);
    if (!is_valid)
        return false;

    block->pos.x = x;    
    block->pos.y = y;    
    return true;
}

void block_draw(block_t* block)
{
    tinygl_point_t* points = block_get_points(block, block->direction);
    for (uint8_t i = 0; i < ARRAY_SIZE(block->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += block->pos.x;
        point.y += block->pos.y;
        tinygl_draw_point(point, 1);
    }
}