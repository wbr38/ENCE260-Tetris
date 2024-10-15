#include "game_data.h"
#include <string.h>

game_data_t* game_data;

void game_data_init()
{
    game_data = malloc(sizeof(game_data_t));
    memset(game_data, 0, sizeof(board_t));

    game_data->game_state = GAME_STATE_MAIN_MENU;
    game_data->rng_seed = 0;
    game_data->board = board_init();
    piece_generate_next(&game_data->current_piece);
    game_data->game_paused = false;
}