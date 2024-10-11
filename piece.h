#ifndef PIECE_H
#define PIECE_H

#include <stdbool.h>
#include "tinygl.h"

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

// Represents a tetris piece (tetromino)
typedef struct
{
    tinygl_point_t points[PIECE_NUM_ROTATIONS][PIECE_NUM_POINTS];
    tinygl_point_t pos;
    orientation_t orientation;
} piece_t;

extern piece_t* current_piece;

bool piece_generate_next(void);
tinygl_point_t* piece_get_points(piece_t* piece, orientation_t orientation);

bool piece_move(piece_t *piece, direction_t direction);
bool piece_rotate(piece_t *piece);
void piece_draw(piece_t *piece);

#endif // PIECE_H