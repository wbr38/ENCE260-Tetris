/** @file game_data.c
 *  @authors William Brown, Matthew Wills
 *  @date 15 October 2024
 *  @brief A structure to contain the current state of the game
 */

#include "game_data.h"

#include "packet.h"
#include <string.h>
#include <timer.h>

game_data_t* game_data;

void game_data_init()
{
    game_data = malloc(sizeof(game_data_t));
    memset(game_data, 0, sizeof(board_t));

    // randomise the prng seed
    srand(timer_get());

    game_data->game_state = GAME_STATE_MAIN_MENU;
    game_data->rng_seed = rand() % PACKET_DATA_MAX_VAL; // rng_seed needs to be networked (in PairingPacket), so can't be larger than packet data.
    game_data->board = board_init();
    piece_generate_next(&game_data->current_piece);
    game_data->our_lines_cleared = 0;
    game_data->their_lines_cleared = 0;
    game_data->game_paused = false;
}