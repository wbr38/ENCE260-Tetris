/** @file board.c
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Represents the current state of the tetris board / the placed pieces.
 */

#include "board.h"
#include <string.h>

board_t* board = 0;

void board_init(void)
{
    board = malloc(sizeof(board_t));
    memset(board, 0, sizeof(board_t));
}

bool board_valid_position(board_t* board, piece_t* piece, uint8_t x, uint8_t y, orientation_t orientation)
{
    const tinygl_point_t* points = piece_get_points(piece, orientation);
    for (uint8_t i = 0; i < PIECE_NUM_POINTS; i++)
    {
        tinygl_point_t point = points[i];
        point.x += x;
        point.y += y;

        // x bounds check
        if (point.x >= TINYGL_WIDTH || point.x < 0)
            return false;

        // y bounds check
        if (point.y >= TINYGL_HEIGHT || point.y < 0)
            return false;

        // this point collides with a placed piece on the board
        if (board->tiles[point.x][point.y])
            return false;
    }

    return true;
}

void board_place_piece(board_t* board, piece_t* piece)
{
    const tinygl_point_t* points = piece_get_points(piece, piece->orientation);

    for (uint8_t i = 0; i < PIECE_NUM_POINTS; i++)
    {
        tinygl_point_t point = points[i];
        point.x += piece->pos.x;
        point.y += piece->pos.y;
        board->tiles[point.x][point.y] = true;
    }
}