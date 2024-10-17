/** @file game_data.h
 *  @authors William Brown, Matthew Wills
 *  @date 15 October 2024
 *  @brief A structure to contain the current state of the game
 */

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "board.h"
#include "piece.h"

typedef enum {
    GAME_STATE_MAIN_MENU,
    GAME_STATE_PAUSED,
    GAME_STATE_STARTING,
    GAME_STATE_PLAYING,
    GAME_STATE_DEAD,
    GAME_SATE_PAUSED,
} game_state_t;

typedef struct {
    game_state_t game_state;

    /** seed used to randomise the order of tetris pieces spawning */
    uint8_t rng_seed;

    board_t* board;

    /** the current tetris piece being placed/controlled */
    piece_t* current_piece;

    /** the total number of lines we have cleared */
    uint8_t our_lines_cleared;

    /** the total number of lines the other player has cleared */
    uint8_t their_lines_cleared;
    
} game_data_t;

/**
 * Global variable of the game state.
 */
extern game_data_t* game_data;

/**
 * Initialise the global variable `game_data`.
 */
void game_data_init(void);

#endif  // GAME_DATA_H