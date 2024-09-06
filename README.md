# Chess in C

This is a GUI Chess application written in C. SDL Rendering is used to display the Chess board and pieces.

Board representation is done using bitboards, which allows for efficient move generation and board evaluation. The AI player uses a negamax algorithm with alpha-beta pruning to search for the best move.

## Features
- Play against an AI opponent.
- Local Two-player mode.
- Load and save board states in FEN format.
- Highlight legal moves.
- Checkmate detection.
- Undo moves.

## Prerequisites

Before you begin, ensure you have installed the following libraries:
- SDL2
- SDL2_image
- SDL2_ttf

These libraries are required to allow for the rendering of the Chess board and pieces.

## Build

To build the Chess application, follow these steps:

1. Clone the repository: 
```
git clone https://github.com/TGasco/ChessInC.git
```
2. Navigate to the project directory: 
```
cd chess
```
3. Install The Required Libraries:
 - SDL2:
    macOS:
    ```
    brew install sdl2
    ```
    Linux (Ubuntu):
    ```
    sudo apt-get install libsdl2-dev
    ```
 - SDL2_image:
    macOS:
    ```
    brew install sdl2_image
    ```
    Linux (Ubuntu):
    ```
    sudo apt-get install libsdl2-image-dev
    ```
 - SDL2_ttf:
    macOS:
    ```
    brew install sdl2_ttf
    ```
    Linux (Ubuntu):
    ```
    sudo apt-get install libsdl2-ttf-dev
    ```
4. Compile the source code using the makefile: 
    ```
    make
    ```
5. The executable file `chess.exe` will be generated.

## Run

To run the Chess application, use the following command:

```
./chess
```

Quit the application at any time by pressing `<ESC>`.

## Command-line Options

The Chess application supports the following command-line options:

- `-h` or `--help`: Display the help message.
- `-b` or `--black`: Play as the black player. (Default is white)
- `-r` or `--random`: Randomize the player colours.
- `-l` or `--load`: Load a board state from file (FEN format). (Example: `./chess -l queens_gambit.txt`)
- `--noai`: Disable the AI player. (Two-player mode)
- `-ai`: AI vs. AI mode. (No player input)
 
Example usage:

For proper command-line argument parsing, and help, please use the provided driver script `driver.sh`.

```
driver.sh [-h] [-b] [-r] [-l <file>] [--noai] [-ai]
```
Which will execute the main application with the specified options after building the project and validating arguments.

Alternatively, you can run the application directly with the following command:
```
./chess [-h] [-b] [-r] [-l <file>] [--noai] [-ai]
```

This will load the position from the file `saved_game.txt` and resume play from that state. (useful for testing positions)

## Contributing

Feel free to open issues and submit pull requests to help improve the project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.