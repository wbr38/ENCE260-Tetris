/** @file board.c
 *  @authors William Brown, Matthew Wills
 *  @date 12 October 2024
 *  @brief Represents the current state of the tetris board / the placed pieces.
 */

#include "board.h"
#include "packet.h"
#include "game_data.h"

#include <string.h>

board_t* board_init(void)
{
    board_t* board = malloc(sizeof(board_t));
    memset(board, 0, sizeof(board_t));
    return board;
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

/** Shift all the rows above the given `row` down */
void board_shift_down(board_t* board, uint8_t row)
{
    for (uint8_t y = row; y > 0; y--)
        for (uint8_t x = 0; x < TINYGL_WIDTH; x++)
            board->tiles[x][y] = board->tiles[x][y - 1];
}

/**
 * @brief Checks and clears any rows that are full. Shifts the board's points appropriately.
 * @param board 
 * @return uint8_t The number of lines cleared
 */
uint8_t board_clear_lines(board_t* board)
{
    uint8_t num_clears = 0;
    for (uint8_t y = 0; y < TINYGL_HEIGHT; y++)
    {
        bool full_row = true;
        for (uint8_t x = 0; x < TINYGL_WIDTH; x++)
        {
            if (!board->tiles[x][y])
            {
                full_row = false;
                break;
            }
        }

        if (full_row)
        {
            board_shift_down(board, y);
            num_clears++;
        }
    }

    return num_clears;
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

    uint8_t num_lines_cleared = board_clear_lines(board);
    game_data->num_lines_cleared += num_lines_cleared;

    // send Line Clear Packet to other board
    packet_t line_clear_packet = {
        .id = LINE_CLEAR_PACKET,
        .data = num_lines_cleared
    };
    packet_send(line_clear_packet);
}