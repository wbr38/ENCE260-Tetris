
#ifndef GRID_H
#define GRID_H

#include "tinygl.h"
#include <stdbool.h>
#include <stdlib.h>

#include "block.h"

typedef struct {
    bool tiles[TINYGL_WIDTH][TINYGL_HEIGHT];
} grid_t;

void grid_init(void);
grid_t* grid_get(void);
void grid_place_block(grid_t* grid, block_t* block);
bool grid_valid_position(grid_t* grid, block_t* block, uint8_t x, uint8_t y, direction_t direction);
#endif // GRID_H