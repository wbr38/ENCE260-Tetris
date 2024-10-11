# TODO
- Add `grid` module:
    - Store the planted tetris pieces
    - Add a function to calculate if a piece position is valid (does it intersect with any placed pieces or the edges of the display)

- Add boundary/collision checks when moving pieces
    - Don't move a piece off the screen
    - Don't move piece into another

- Add task for automatically updating piece position
    - automatically move pieces down
    - if the next move down is invalid, place the piece on the grid
    - then generate the next piece to spawn
    - ^ somewhere, have a check if the game is over. I.e. will the next piece to spawn be a valid position or not