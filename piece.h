/** @file piece.h
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Represents a tetris piece/block/tetromino.
 */

#ifndef PIECE_H
#define PIECE_H

#include <stdbool.h>
#include "tinygl.h"

#define PIECES_COUNT        7 // total number of tetris pieces
#define PIECE_NUM_ROTATIONS 4 // each piece has precalculated 4 rotations
#define PIECE_NUM_POINTS    4 // each piece is defined with 4 pixel points

typedef enum
{
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction_t;

typedef enum
{
    ORIENTATION_NORTH,
    ORIENTATION_SOUTH,
    ORIENTATION_WEST,
    ORIENTATION_EAST
} orientation_t;

/** Represents a tetris piece (tetromino) */
typedef struct
{
    tinygl_point_t points[PIECE_NUM_ROTATIONS][PIECE_NUM_POINTS];
    tinygl_point_t pos;
    orientation_t orientation;
} piece_t;

/** The piece that is currently being placed/controlled */
extern piece_t* current_piece;

/**
 * @brief Spawn the next piece (`current_piece`)
 * @return Whether the piece spawned at a valid position. If this function returns 
 *         `false`, then the game is over.
 */
bool piece_generate_next(void);

/** Returns a tinygl_point_t array for the given orientation of this piece. */
tinygl_point_t* piece_get_points(piece_t* piece, orientation_t orientation);

/**
 * @brief Attempt to move the piece in the given direction.
 * @return true if the piece was successfully moved.
 * @return false if the piece was not able to be moved in the given direction (e.g. would collide a wall).
 */
bool piece_move(piece_t *piece, direction_t direction);

/**
 * @brief Attempt to rotate the piece clockwise.
 * @return true if the piece was succesfully rotated.
 * @return false if the piece was not able to be rotated (e.g. would collide with a wall).
 */
bool piece_rotate(piece_t *piece);

/**
 * @brief Draws the given piece on the LED matrix.
 */
void piece_draw(piece_t *piece);

#endif // PIECE_H