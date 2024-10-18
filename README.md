# Tetris

## Authors
- William Brown (wbr38)
- Matthew Wills (mwi158)

## Description
This is a 2 player game, opponents play simultaneously on their own device. Blocks spawn at the top of the screen and fall down, place the blocks in full rows to clear the row and earn points. Stacking blocks too high will end your round. Whoever has the most points at the end of their round wins. The blue led will flash when the opponent clears a row. After both opponents have completed a round, results are displayed and they can reset and play another round.

## Set Up
Clone the UCFK4 repo:
```bash
$ git clone https://eng-git.canterbury.ac.nz/mph/ence260-ucfk4
``` 

Clone this repository into `ence260-ucfk4/assignment`
```bash
$ cd ence260-ucfk4/assignment
$ git clone https://eng-git.canterbury.ac.nz/ence260-2024/group_318
```

Plug in your UCFK4 and build the program
```bash
$ make program
```

To play Tetris on the UCFK4, place the 2 devices so that they can communicate, with infared recievers and transmitters facing eachother. Run the program on both devices.

## Controls
Press the nav switch while "tetris" is scrolling on screen to start the countdown. once the countdown ends, gameplay will start.

Move the block left by using west on the joystick.
Move the block right by using east on the joystick.
Move the block down by using south on the joystick.
Rotate the block by pressing the nav switch.

Press the nav switch after a round is over to reset the game to the main "tetris" screen.