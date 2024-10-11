
#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdlib.h>
#include "tinygl.h"

#include "piece.h"

typedef struct {
    bool tiles[TINYGL_WIDTH][TINYGL_HEIGHT];
} board_t;

extern board_t* board;

void board_init(void);
void board_place_piece(board_t* board, piece_t* piece);
bool board_valid_position(board_t* board, piece_t* piece, uint8_t x, uint8_t y, direction_t direction);
#endif // BOARD_H