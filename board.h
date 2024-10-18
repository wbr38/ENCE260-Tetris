/** @file board.h
 *  @authors William Brown (wbr38), Matthew Wills (mwi158)
 *  @date 12 October 2024
 *  @brief Represents the current state of the tetris board / the placed pieces.
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdlib.h>
#include <tinygl.h>

#include "piece.h"

typedef struct {
    bool tiles[TINYGL_WIDTH][TINYGL_HEIGHT];
} board_t;

/**
 * @brief Initialises the board state
 * @param board Pass by reference `board` to be initialized
 */
void board_init(board_t** board);

/**
 * @brief Place the given tetris piece at its current position on the board
 * @param board The board to place the piece on. (`game_data->board`)
 * @param piece The piece to be placed on the board. (`game_data->piece`)
 */
void board_place_piece(board_t* board, piece_t* piece);

/**
 * @returns whether the given tetris piece, at the given coordinates and orientation, is at a valid position on the board.
 *          i.e. it does not collide with any placed pieces or extend outside the bounds of the LED display.
 *
 * @param board The global `board` object
 * @param piece The piece to check is valid
 * @param x The x coordinate of the piece to test
 * @param y The y coordinate of the piece to test
 * @param orientation The orientation of the piece to test
 */
bool board_valid_position(board_t* board, piece_t* piece, uint8_t x, uint8_t y, orientation_t orientation);
#endif  // BOARD_H