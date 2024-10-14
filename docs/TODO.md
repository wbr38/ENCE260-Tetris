# TODO

- Add IR/packet functionality
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