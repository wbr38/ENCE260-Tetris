
#include "grid.h"

grid_t* grid = 0;

void grid_init(void)
{
    grid = malloc(sizeof(grid_t));

    // Initialize all tiles to false
    for (uint8_t x = 0; x < TINYGL_WIDTH; x++) {
        for (uint8_t y = 0; y < TINYGL_HEIGHT; y++) {
            grid->tiles[x][y] = false;
        }
    }
}

bool grid_valid_position(grid_t* grid, piece_t* piece, uint8_t x, uint8_t y, direction_t direction)
{
    tinygl_point_t* points = piece_get_points(piece, direction);
    for (uint8_t i = 0; i < ARRAY_SIZE(piece->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += x;
        point.y += y;
        tinygl_draw_point (point, 1);
        if (grid->tiles[point.x][point.y])
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

void grid_place_piece(grid_t* grid, piece_t* piece)
{
    tinygl_point_t* points = piece_get_points(piece, piece->direction);

    for (uint8_t i = 0; i < ARRAY_SIZE(piece->points); i++)
    {
        tinygl_point_t point = points[i];
        point.x += piece->pos.x;
        point.y += piece->pos.y;
        grid->tiles[point.x][point.y] = true;
    }
}