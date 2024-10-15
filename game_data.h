/** @file game_data.h
 *  @authors William Brown, Matthew Wills
 *  @date 15 October 2024
 *  @brief 
 */

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <stdint.h>

#include "piece.h"
#include "board.h"

typedef enum {
    GAME_STATE_MAIN_MENU,
    GAME_STATE_PAUSED,
    GAME_STATE_PLAYING,
    GAME_STATE_DEAD,
} game_state_t;

typedef struct {
    game_state_t game_state;
    uint8_t rng_seed;
    board_t* board;
    piece_t* current_piece;
    bool game_paused;
} game_data_t;


extern game_data_t* game_data;
void game_data_init(void);

#endif // GAME_DATA_H