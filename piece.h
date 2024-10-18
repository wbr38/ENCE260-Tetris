/** @file piece.h
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Represents a tetris piece/block/tetromino.
 */

#ifndef PIECE_H
#define PIECE_H

#include <stdbool.h>
#include <tinygl.h>

#define PIECES_COUNT        7  // total number of tetris pieces
#define PIECE_NUM_ROTATIONS 4  // each piece has precalculated 4 rotations
#define PIECE_NUM_POINTS    4  // each piece is defined with 4 pixel points
#define PIECE_GRID_SIZE     4  // we define each piece's points on a 4x4 grid.

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction_t;

typedef enum {
    ORIENTATION_NORTH,
    ORIENTATION_SOUTH,
    ORIENTATION_WEST,
    ORIENTATION_EAST
} orientation_t;

/** Represents a tetris piece (tetromino) */
typedef struct
{
    uint8_t idx;
    tinygl_point_t pos;
    orientation_t orientation;
} piece_t;

/**
 * @brief Spawn/initialise the next tetris piece into `current_piece`.
 * @param current_piece Pass by reference the pointer to the current_piece.
 * @return Whether the piece spawned at a valid position. If this function returns
 *         `false`, then the game is over.
 */
bool piece_generate_next(piece_t** current_piece);

/**
 * @brief Returns a tinygl_point_t array for the given orientation of this piece.
 */
const tinygl_point_t* piece_get_points(piece_t* piece, uint8_t x, uint8_t y, orientation_t orientation);

/**
 * @brief Attempt to move the piece in the given direction.
 * @return true if the piece was successfully moved.
 * @return false if the piece was not able to be moved in the given direction (e.g. would collide a wall).
 */
bool piece_move(piece_t* piece, direction_t direction);

/**
 * @brief Attempt to rotate the piece clockwise.
 * @return true if the piece was succesfully rotated.
 * @return false if the piece was not able to be rotated (e.g. would collide with a wall).
 */
bool piece_rotate(piece_t* piece);

/**
 * @brief Draws the given piece on the LED matrix.
 */
void piece_draw(piece_t* piece);

#endif  // PIECE_H