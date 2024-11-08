/** @file piece.c
 *  @authors William Brown (wbr38), Matthew Wills (mwi158)
 *  @date 12 October 2024
 *  @brief Represents a tetris piece/block/tetromino.
 */

#include "piece.h"

#include <navswitch.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "game_data.h"

// Simply combines 4 params (each param should just be be 4 bits) into a single binary string, for easier visualation.
#define BINARY(a, b, c, d) 0b##a##b##c##d

/**
 * Precalculated tetris pieces and their rotations. (see image: https://harddrop.com/wiki/File:SRS-pieces.png)
 * Every piece always has exactly 4 points, so we define each piece on a 4x4 grid.
 * This fits nicely (4x4 = 16 bits) into a uint16_t.
 */
const uint16_t pieces[PIECES_COUNT][PIECE_NUM_ROTATIONS] = {
    // clang-format off

    // I Piece
    {
        BINARY( 0000,
                1111,
                0000,
                0000),
        
        BINARY( 0010,
                0010,
                0010,
                0010),

        BINARY( 0000,
                0000,
                1111,
                0000),

        BINARY( 0100,
                0100,
                0100,
                0100),
    },

    // J Piece
    {
        BINARY( 1000,
                1110,
                0000,
                0000),
        
        BINARY( 0110,
                0100,
                0100,
                0000),

        BINARY( 0000,
                1110,
                0010,
                0000),

        BINARY( 0100,
                0100,
                1100,
                0000),
    },

    // L Piece
    {
        BINARY( 0010,
                1110,
                0000,
                0000),
        
        BINARY( 0100,
                0100,
                0110,
                0000),

        BINARY( 0000,
                1110,
                1000,
                0000),

        BINARY( 1100,
                0100,
                0100,
                0000),
    },
    
    // O Piece
    {
        BINARY( 0110,
                0110,
                0000,
                0000),

        BINARY( 0110,
                0110,
                0000,
                0000),

        BINARY( 0110,
                0110,
                0000,
                0000),

        BINARY( 0110,
                0110,
                0000,
                0000),
    },

    // S Piece
    {
        BINARY( 0110,
                1100,
                0000,
                0000),
        
        BINARY( 0100,
                0110,
                0010,
                0000),

        BINARY( 0000,
                0110,
                1100,
                0000),

        BINARY( 1000,
                1100,
                0100,
                0000),
    },

    // T Piece
    {
        BINARY( 0100,
                1110,
                0000,
                0000),
        
        BINARY( 0100,
                0110,
                0100,
                0000),

        BINARY( 0000,
                1110,
                0100,
                0000),

        BINARY( 0100,
                1100,
                0100,
                0000),
    },

    // Z Piece
    {
        BINARY( 1100,
                0110,
                0000,
                0000),
        
        BINARY( 0010,
                0110,
                0100,
                0000),

        BINARY( 0000,
                1100,
                0110,
                0000),

        BINARY( 0100,
                1100,
                1000,
                0000),
    },

    // clang-format on
};

/**
 * @brief Randomly shuffle the itmes in the given array.
 */
static void shuffle_array(uint8_t* arr, size_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t j = rand() % size;
        uint8_t temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

/**
 * @brief Spawn/initialise the next tetris piece into `current_piece`.
 * @param current_piece Pass by reference the pointer to the current_piece.
 * @return Whether the piece spawned at a valid position. If this function returns
 *         `false`, then the game is over.
 */
bool piece_generate_next(piece_t** current_piece)
{
    static uint8_t _nextPieceId = 0;

    // On first call, randomise the indexes used for the `pieces` array.
    static bool init = false;
    static uint8_t pieceIdx[PIECES_COUNT] = {0, 1, 2, 3, 4, 5, 6};
    if (!init)
    {
        shuffle_array(pieceIdx, ARRAY_SIZE(pieceIdx));

        // On first run we malloc the current_piece pointer, and continue to reuse it for the rest of the program
        *current_piece = malloc(sizeof(piece_t));
        init = true;
    }

    piece_t* piece = *current_piece;
    memset(piece, 0, sizeof(piece_t));

    // set values
    piece->idx = pieceIdx[_nextPieceId];
    piece->orientation = ORIENTATION_NORTH;
    piece->pos = (tinygl_point_t){
        .x = 1,  // offset by 1 so pieces spawn centered
        .y = 0,
    };

    _nextPieceId = (_nextPieceId + 1) % ARRAY_SIZE(pieces);

    // check if the new current_piece pos is valid
    bool valid_pos = board_valid_position(
        game_data->board,
        piece,
        piece->pos.x,
        piece->pos.y,
        piece->orientation);

    return valid_pos;
}

/**
 * @brief Returns a tinygl_point_t array for the given orientation of this piece.
 */
const tinygl_point_t* piece_get_points(piece_t* piece, uint8_t x, uint8_t y, orientation_t orientation)
{
    static tinygl_point_t points[PIECE_NUM_POINTS];
    uint16_t pattern = pieces[piece->idx][orientation];

    // As stated above, each tetris piece is defined on a 4x4 grid.
    // We start with the left most bit, and continually shift it to the right, to test if the bit
    // in the given position is 1, indicating a point.
    // think of this uint16_t as groupings of 4 bits: 4 columns and 4 rows (see above how the pieces array is defined)

    uint16_t test_bit = 0b1000000000000000;

    uint8_t i = 0;
    for (uint8_t column = 0; column < PIECE_GRID_SIZE; column++)
    {
        for (uint8_t row = 0; row < PIECE_GRID_SIZE; row++)
        {
            // if there is a 1 in this bit pos
            if (pattern & test_bit)
            {
                points[i].x = row + x;
                points[i].y = column + y;
                i++;
            }

            test_bit >>= 1;
        }
    }

    return points;
}

/**
 * @brief Attempt to rotate the piece clockwise.
 * @return true if the piece was succesfully rotated.
 * @return false if the piece was not able to be rotated (e.g. would collide with a wall).
 */
bool piece_rotate(piece_t* piece)
{
    orientation_t new_orientation = (piece->orientation + 1) % PIECE_NUM_ROTATIONS;

    bool is_valid = board_valid_position(game_data->board, piece, piece->pos.x, piece->pos.y, new_orientation);
    if (!is_valid)
        return false;

    piece->orientation = new_orientation;
    return true;
}

/**
 * @brief Attempt to move the piece in the given direction.
 * @return true if the piece was successfully moved.
 * @return false if the piece was not able to be moved in the given direction (e.g. would collide a wall).
 */
bool piece_move(piece_t* piece, direction_t direction)
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

    // tetris pieces are never moved upwards
    case DIRECTION_UP:
        break;

    default:
        break;
    }

    // Check this new position is valid
    bool is_valid = board_valid_position(game_data->board, piece, x, y, piece->orientation);
    if (!is_valid)
        return false;

    piece->pos.x = x;
    piece->pos.y = y;
    return true;
}

/**
 * @brief Draws the given piece on the LED matrix.
 */
void piece_draw(piece_t* piece)
{
    const tinygl_point_t* points = piece_get_points(piece, piece->pos.x, piece->pos.y, piece->orientation);
    for (uint8_t i = 0; i < PIECE_NUM_POINTS; i++)
    {
        tinygl_point_t point = points[i];
        tinygl_draw_point(point, 1);
    }
}