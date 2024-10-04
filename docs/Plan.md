# Tetris Versus
1v1 tetris game


## Startup:
* Determine a random seed to change the order of the tetris pieces spawning

## Pairing
* When push button is held, send the RNG seed to the other board
* The other board (not holding button) will recv the RNG seed and respond with an acknowledgement packet.
* The game will then commence in 3 2 1 countdown

```
PairingPacket       - [id] [rng_seed]
PairingAckPacket    - [id]
```

## Game
* When a line is cleared, the blue LED on the other UCFK4 will flash for how many lines were cleared
* At a constant rate, send and receive Ping/Pong packets
* When we fail to receive a Pong packet, the game should pause.
* Maybe these should include a serial, or a timestamp?

```
LineClearPacket     - [id] [num_rows_cleared]
Ping - [id]
Pong - [id]
```

## Finish
* When a player dies, a packet is sent
* ^ this packet should be repeatedly sent until we receive an acknowledgement
* The total score is how many lines were cleared

* We could make clearing multiple lines worth more points
* So instead of simply ending the game as soon as one person dies
* We wait for both to die, they will send their score (total lines cleared) to the other player

```
DiePacket - [id] [total_clears]
DiePacketAck - [id]
```

* We then show a WIN / LOSE screen
* Maybe with total score shown



## Extra
To increase marks for Interactivity & Complexity:

### Spectator Mode:
* When a player dies, instead of just waiting for the other player to die, they could become a 'spectator'
* The other players board state is communicated and shown on the other board

### Powerups/Attacks:
* When multiple lines a cleared, the other player could:
    * game speed increased
    * garbage lines added

maybe too chaotic for such a small LED screen