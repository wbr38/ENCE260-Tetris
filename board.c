/** @file board.c
 *  @authors William Brown (wbr38), Matthew Wills (mwi158)
 *  @date 12 October 2024
 *  @brief Represents the current state of the tetris board / the placed pieces.
 */

#include "board.h"

#include <string.h>

#include "game_data.h"
#include "packet.h"

/**
 * @brief Initialises the board state
 * @param board Pass by reference `board` to be initialized
 */
void board_init(board_t** board)
{
    // On first run we malloc the board pointer, and continue to reuse it for the rest of the program
    static bool init = false;
    if (!init)
    {
        *board = malloc(sizeof(board_t));
        init = true;
    }

    memset(*board, 0, sizeof(board_t));

    // board->pieces are all set to 0 from above call to memset
    // nothing else to do here
}

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
bool board_valid_position(board_t* board, piece_t* piece, uint8_t x, uint8_t y, orientation_t orientation)
{
    const tinygl_point_t* points = piece_get_points(piece, x, y, orientation);
    for (uint8_t i = 0; i < PIECE_NUM_POINTS; i++)
    {
        tinygl_point_t point = points[i];

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

/**
 * @brief Shift all the rows above the given `row` down
 * @param board The board object, (`game_data->board`)
 * @param row The rows above this `row` will be shifted down.
 *            This will essentially the clear this row.
 */
void board_shift_down(board_t* board, uint8_t row)
{
    for (uint8_t y = row; y > 0; y--)
        for (uint8_t x = 0; x < TINYGL_WIDTH; x++)
            board->tiles[x][y] = board->tiles[x][y - 1];
}

/**
 * @brief Checks and clears any rows that are full. Shifts the board's points appropriately.
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

/**
 * @brief Place the given tetris piece at its current position on the board
 * @param board The board to place the piece on. (`game_data->board`)
 * @param piece The piece to be placed on the board. (`game_data->piece`)
 */
void board_place_piece(board_t* board, piece_t* piece)
{
    const tinygl_point_t* points = piece_get_points(piece, piece->pos.x, piece->pos.y, piece->orientation);

    for (uint8_t i = 0; i < PIECE_NUM_POINTS; i++)
    {
        tinygl_point_t point = points[i];
        board->tiles[point.x][point.y] = true;
    }

    uint8_t lines_cleared = board_clear_lines(board);
    game_data->our_lines_cleared += lines_cleared;

    // send Line Clear Packet to other board
    packet_t line_clear_packet = {
        .id = LINE_CLEAR_PACKET,
        .data = lines_cleared,
    };
    packet_send(line_clear_packet);
}