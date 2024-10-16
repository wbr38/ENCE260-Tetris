# TODO

- Make nav push rotate instead of button1
    use button1 for dev testing, otherwise we end up blocking the IR receiver

- Add Ping Pong (pause)

    Pausing functionality:
        Add GAME_STATE_PAUSED
        The button_task and board_move_down_task can probably be left untouched
        Just add code to display_task to show the game is paused
        Test this works initially before adding IR stuff

    Add a bool "host" to game_data, this will be used to determine if we should send a Ping packet, or just respond with Pong packet.
    Set this to true if we send the Pairing packet
    Set this to false if we receive PairingAck

    Add a bool to game_data called "recvd_pingpong"
    This will be used to check if we have received a ping/pong packet by the time this task runs again

    In handle_packet:
        if this is a Pong packet, and we are host
            set game_data->recvd_pingpong to true
        
        if this is a Ping packet, and we are not host:
            respond with the Pong packet
            set game_data->recvd_pingpong to true

    Add a new task: ping_pong_task
        With a frequency of 2 (500ms)
        This will be used for both boards to check if they have recvd the required ping or pong packet in time
        And for the host to send Ping

        if game_data->recvd_pingpong is false
            set game_data->game_state to GAME_STATE_PAUSED
            return
        
        if game_data->host
            send Ping packet

        set game_data->recvd_pingpong to false

- Add Die/DieAck packet
    probably need another game state
    DEAD - we are dead, other player still alive so game is ongoing
    GAME_OVER - both players are dead

- Add more func docs, at least to all functions in header files

- Add README (check marking guide for what this needs to contain)

```
Packet Structure:
    [id]    - 3 bits, 2^3 = 8 distinct values. (0->7) packets
    [data]  - 5 bits, 2^5 = 32 distinct values. (0->31)

    OR for spectator:
    [id]    - 4 bits, 2^4 = 16 distinct values. (0->15) packets
    [data]  - 4 bits, 2^4 = 16 distinct values. (0->15) 

Packets:
    Pairing         - [id] [rng_seed]
    PairingAck      - [id]

    Ping            - [id]
    Pong            - [id]

    LineClear       - [id] [num_rows]

    Die             - [id]
    DieAck          - [id]

    Spectator:
        PieceSpawn  - [id] [piece_id]
        PieceMoveX  - [id] [x]
        PieceMoveY  - [id] [y]
        PiecePlace  - [id]
```