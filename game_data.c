/** @file game_data.c
 *  @authors William Brown (wbr38), Matthew Wills (mwi158)
 *  @date 15 October 2024
 *  @brief A structure to contain the current state of the game
 */

#include "game_data.h"

#include "packet.h"
#include <string.h>
#include <timer.h>

/**
 * Global variable of the game state.
 */
game_data_t* game_data = NULL;

/**
 * Initialise the global variable `game_data`.
 */
void game_data_init()
{
    static bool init = false;
    if (!init)
    {
        game_data = malloc(sizeof(game_data_t));
        memset(game_data, 0, sizeof(game_data_t));
        init = true;
    }

    // randomise the prng seed
    srand(timer_get());

    game_data->game_state = GAME_STATE_MAIN_MENU;
    game_data->host = false;
    game_data->rng_seed = rand() % PACKET_DATA_MAX_VAL; // rng_seed needs to be networked (in PairingPacket), so can't be larger than packet data.
    board_init(&game_data->board);
    piece_generate_next(&game_data->current_piece);
    game_data->our_lines_cleared = 0;
    game_data->their_lines_cleared = 0;
    game_data->die_packet_acknowledged = false;
    game_data->other_player_dead = false;
    game_data->recvd_pingpong = true; // initialise with true so game doesn't immediately pause
}

/**
 * This function checks if both players have died, then sets the game state to GAME_OVER.
 */
void game_data_check_game_over(void)
{
    // both players dead, game is over
    if (game_data->game_state == GAME_STATE_DEAD && game_data->other_player_dead)
        game_data->game_state = GAME_STATE_GAME_OVER;
}

/**
 * This function checks if the game should be paused, if this board has not received the
 * required ping or pong packet.
 */
void game_data_check_pause(void)
{
    // The host will always send a Ping packet
    // In handle_packet, we set `game_data->recv_pingpong` to true when a Ping/Pong packet has been received

    // Didn't recv a pingpong. Only if we are playing, pause the game.
    if (!game_data->recvd_pingpong && game_data->game_state == GAME_STATE_PLAYING)
        game_data->game_state = GAME_STATE_PAUSED;

    // We did recv a pingpong. Unpause the game if we are currently paused
    // We only ever pause when we are playing, so unpause by setting state back to playing.
    if (game_data->recvd_pingpong && game_data->game_state == GAME_STATE_PAUSED)
        game_data->game_state = GAME_STATE_PLAYING;

    game_data->recvd_pingpong = false;

}