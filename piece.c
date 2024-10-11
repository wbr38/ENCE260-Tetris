#include "piece.h"
#include "board.h"

#include <string.h>
#include <stdlib.h>

#include "navswitch.h"

#define PIECES_COUNT 7  // 7 total tetris pieces

piece_t* current_piece = 0; // The current piece being placed

// All tetris pieces and their rotations
// Positions are relative, centered around a point (see image: https://cdn.harddrop.com/1/17/SRS-true-rotations.png)
// Left=Negative, Right=Positive, Up=Negative, Down=Positive
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

    piece_t* piece = malloc(sizeof(piece_t));
    memcpy(piece->points, pieces[_nextPieceId], sizeof(piece->points));
    piece->direction = DIRECTION_UP;

    piece->pos = (tinygl_point_t){
        .x = TINYGL_WIDTH / 2, // spawn piece initially in center
        .y = 1
    };

    current_piece = piece;

    _nextPieceId++;
    if (_nextPieceId >= ARRAY_SIZE(pieces))
        _nextPieceId = 0;

    // check if current_piece pos is valid
    bool valid_pos = board_valid_position(board, current_piece, current_piece->pos.x, current_piece->pos.y, current_piece->direction);
    return valid_pos;
}

tinygl_point_t* piece_get_points(piece_t* piece, direction_t direction)
{
    return piece->points[direction];
}

void piece_rotate(piece_t *piece)
{
    direction_t new_dir = (piece->direction + 1) % PIECE_NUM_ROTATIONS;

    bool is_valid = board_valid_position(board, piece, piece->pos.x, piece->pos.y, new_dir);
    if (!is_valid)
        return;

    piece->direction = new_dir;
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

    // TODO: Check new position is valid. is_valid_pos(points, x, y, piece->direction)
    
    bool is_valid = board_valid_position(board, piece, x, y, piece->direction);
    if (!is_valid)
        return false;

    piece->pos.x = x;    
    piece->pos.y = y;    
    return true;
}

void piece_draw(piece_t* piece)
{
    tinygl_point_t* points = piece_get_points(piece, piece->direction);
    for (uint8_t i = 0; i < ARRAY_SIZE(piece->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += piece->pos.x;
        point.y += piece->pos.y;
        tinygl_draw_point(point, 1);
    }
}