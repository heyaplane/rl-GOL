This project can be built by installing Raylib (https://github.com/raysan5/raylib) and running the following command on Linux from the root project dir:

gcc gol.c -o gol -lraylib -lm


The files included in this project:

- gol.c:
  - Contains the main event loop and dispatches the compute/render shader on each frame.
  - Also contains code for parsing the initial board state, converting the recorded frame counts to binary digits, and writing the bits to a .csv file.

- shaders/gol.glsl:
  - Compute shader for updating the state of the board each frame.
  - Also selects one cell to act as a "flag" for recording the frame count. This cell needs to be updated depending on the "rle_string" that is uncommented in gol.c.

- shaders/gol_render.glsl:
  - Renders the board each frame using a texture.
  - The flagged cell can be highlighted red for visualization by uncommenting either of the last lines of code, depending on the "rle_string" that is uncommented in gol.c.


Copies of the spreadsheets used to perform the statistical calculations are also included.
