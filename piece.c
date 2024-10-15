/** @file piece.c
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Represents a tetris piece/block/tetromino.
 */

#include "piece.h"
#include "board.h"
#include "game_data.h"

#include <string.h>
#include <stdlib.h>

#include "navswitch.h"

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

bool piece_generate_next(piece_t** current_piece)
{
    static uint8_t _nextPieceId = 0;

    if (*current_piece == NULL)
        *current_piece = malloc(sizeof(piece_t));

    piece_t* piece = *current_piece;
    memset(piece, 0, sizeof(piece_t));

    // set values
    piece->id = _nextPieceId;
    piece->orientation = ORIENTATION_NORTH;
    piece->pos = (tinygl_point_t){
        .x = TINYGL_WIDTH / 2, // spawn piece initially in center
        .y = 1
    };

    _nextPieceId = (_nextPieceId + 1) % ARRAY_SIZE(pieces);

    // check if the new current_piece pos is valid
    bool valid_pos = board_valid_position(
        game_data->board,
        piece,
        piece->pos.x,
        piece->pos.y,
        piece->orientation
    );
    return valid_pos;
}

const tinygl_point_t* piece_get_points(piece_t* piece, orientation_t orientation)
{
    return pieces[piece->id][orientation];
}

bool piece_rotate(piece_t *piece)
{
    orientation_t new_orientation = (piece->orientation + 1) % PIECE_NUM_ROTATIONS;

    bool is_valid = board_valid_position(game_data->board, piece, piece->pos.x, piece->pos.y, new_orientation);
    if (!is_valid)
        return false;

    piece->orientation = new_orientation;
    return true;
}

bool piece_move(piece_t *piece, direction_t direction)
{
    int8_t x = piece->pos.x;
    int8_t y = piece->pos.y;

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
    bool is_valid = board_valid_position(game_data->board, piece, x, y, piece->orientation);
    if (!is_valid)
        return false;

    piece->pos.x = x;    
    piece->pos.y = y;    
    return true;
}

void piece_draw(piece_t* piece)
{
    const tinygl_point_t* points = piece_get_points(piece, piece->orientation);
    for (uint8_t i = 0; i < PIECE_NUM_POINTS; i++)
    {
        tinygl_point_t point = points[i];
        point.x += piece->pos.x;
        point.y += piece->pos.y;
        tinygl_draw_point(point, 1);
    }
}