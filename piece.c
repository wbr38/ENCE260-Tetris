/** @file piece.c
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Represents a tetris piece/block/tetromino.
 */

#include "piece.h"
#include "board.h"

#include <string.h>
#include <stdlib.h>

#include "navswitch.h"

piece_t* current_piece = 0; 

/**
 * All tetris pieces and their rotations
 * Positions are relative, centered around a point (see image: https://cdn.harddrop.com/1/17/SRS-true-rotations.png)
 * Left=Negative, Right=Positive, Up=Negative, Down=Positive
 */
const tinygl_point_t pieces[PIECES_COUNT][PIECE_NUM_ROTATIONS][PIECE_NUM_POINTS] = {

    // I Piece
    {
        { {-1, 0}, {0, 0}, {1, 0}, {2, 0} },
        { {0, -1}, {0, 0}, {0, 1}, {0, 2} },
        { {-2, 0}, {-1, 0}, {0, 0}, {1, 0} },
        { {0, -2}, {0, -1}, {0, 0}, {0, 1} },
    },

    // J Piece
    {
        { {-1, -1}, {-1, 0}, {0, 0}, {1, 0} },
        { {1, -1}, {0, -1}, {0, 0}, {0, 1} },
        { {-1, 0}, {0, 0}, {1, 0}, {1, 1} },
        { {-1, 1}, {0, 1}, {0, 0}, {0, -1} },
    },

    // L Piece
    {
        { {-1, 0}, {0, 0}, {1, 0}, {1, -1} },
        { {0, 1}, {0, 0}, {0, -1}, {1, 1} },
        { {-1, 1}, {-1, 0}, {0, 0}, {1, 0} },
        { {-1, -1}, {0, -1}, {0, 0}, {0, 1} },
    },

    // O (Square) Piece
    {
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
        { {0, 0}, {0, -1}, {1, -1}, {1, 0} },
    },

    // S Piece
    {
        { {-1, 0}, {0, 0}, {0, -1}, {1, -1} },
        { {0, -1}, {0, 0}, {1, 0}, {1, 1} },
        { {-1, 1}, {0, 1}, {0, 0}, {1, 0} },
        { {-1, 0}, {-1, -1}, {0, 0}, {0, 1} },
    },

    // T Piece
    {
        { {-1, 0}, {0, 0}, {0, -1}, {1, 0} },
        { {0, -1}, {0, 0}, {0, 1}, {1, 0} },
        { {-1, 0}, {0, 0}, {1, 0}, {0, 1} },
        { {-1, 0}, {0, 0}, {0, -1}, {0, 1} },
    },

    // Z Piece
    {
        { {-1, -1}, {0, -1}, {0, 0}, {1, 0} },
        { {0, 1}, {0, 0}, {1, 0}, {1, -1} },
        { {-1, 0}, {0, 0}, {0, 1}, {1, 1} },
        { {-1, 0}, {-1, 1}, {0, 0}, {0, -1} },
    },
};

bool piece_generate_next()
{
    static uint8_t _nextPieceId = 0;

    if (current_piece != NULL)
        free(current_piece);

    current_piece = malloc(sizeof(piece_t));
    memcpy(current_piece->points, pieces[_nextPieceId], sizeof(current_piece->points));
    current_piece->orientation = ORIENTATION_NORTH;

    current_piece->pos = (tinygl_point_t){
        .x = TINYGL_WIDTH / 2, // spawn piece initially in center
        .y = 1
    };

    _nextPieceId = (_nextPieceId + 1) % ARRAY_SIZE(pieces);

    // check if the new current_piece pos is valid
    bool valid_pos = board_valid_position(board, current_piece, current_piece->pos.x, current_piece->pos.y, current_piece->orientation);
    return valid_pos;
}

tinygl_point_t* piece_get_points(piece_t* piece, orientation_t orientation)
{
    return piece->points[orientation];
}

bool piece_rotate(piece_t *piece)
{
    orientation_t new_orientation = (piece->orientation + 1) % PIECE_NUM_ROTATIONS;

    bool is_valid = board_valid_position(board, piece, piece->pos.x, piece->pos.y, new_orientation);
    if (!is_valid)
        return false;

    piece->orientation = new_orientation;
    return true;
}

bool piece_move(piece_t *piece, direction_t direction)
{
    int x = piece->pos.x;
    int y = piece->pos.y;

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

    // Check this new position is valid
    // TODO: Try to "kick" the piece into place
    bool is_valid = board_valid_position(board, piece, x, y, piece->orientation);
    if (!is_valid)
        return false;

    piece->pos.x = x;    
    piece->pos.y = y;    
    return true;
}

void piece_draw(piece_t* piece)
{
    tinygl_point_t* points = piece_get_points(piece, piece->orientation);
    for (uint8_t i = 0; i < ARRAY_SIZE(piece->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += piece->pos.x;
        point.y += piece->pos.y;
        tinygl_draw_point(point, 1);
    }
}