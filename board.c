#include "board.h"

board_t* board = 0;

void board_init(void)
{
    board = malloc(sizeof(board_t));

    // Initialize all tiles to false
    for (uint8_t x = 0; x < TINYGL_WIDTH; x++) {
        for (uint8_t y = 0; y < TINYGL_HEIGHT; y++) {
            board->tiles[x][y] = false;
        }
    }
}

bool board_valid_position(board_t* board, piece_t* piece, uint8_t x, uint8_t y, direction_t direction)
{
    tinygl_point_t* points = piece_get_points(piece, direction);
    for (uint8_t i = 0; i < ARRAY_SIZE(piece->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += x;
        point.y += y;
        tinygl_draw_point (point, 1);
        if (board->tiles[point.x][point.y])
        {
            return false;
        }

        if (point.x >= TINYGL_WIDTH || point.x < 0) {
            return false;
        }

        if (point.y >= TINYGL_HEIGHT || point.y < 0) {
            return false;
        }
    }

    return true;
}

void board_place_piece(board_t* board, piece_t* piece)
{
    tinygl_point_t* points = piece_get_points(piece, piece->direction);

    for (uint8_t i = 0; i < ARRAY_SIZE(piece->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += piece->pos.x;
        point.y += piece->pos.y;
        board->tiles[point.x][point.y] = true;
    }
}