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
    /** Main menu of the game, players need to pair before starting */
    GAME_STATE_MAIN_MENU,

    /** Game is paused, lost signal with the other board */
    GAME_STATE_PAUSED,

    /** The game is about to start (3 2 1 countdown) */
    GAME_STATE_STARTING,

    /** Game is currently being played */
    GAME_STATE_PLAYING,

    /** We are dead but the other player is still alive */
    GAME_STATE_DEAD,

    /** Both players are dead, game is over. */
    GAME_STATE_GAME_OVER,
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

    /** Used when we die to repeatedly sent the Die packet until the board sends the Ack packet */
    bool die_packet_acknowledged;

    /** Is the other player still alive/playing */
    bool other_player_dead;
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