
#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdlib.h>
#include "tinygl.h"

#include "block.h"

typedef struct {
    bool tiles[TINYGL_WIDTH][TINYGL_HEIGHT];
} grid_t;

extern grid_t* grid;

void grid_init(void);
void grid_place_block(grid_t* grid, block_t* block);
bool grid_valid_position(grid_t* grid, block_t* block, uint8_t x, uint8_t y, direction_t direction);
#endif // GRID_H