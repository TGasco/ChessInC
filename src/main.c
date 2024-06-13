#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
// #include <SDL_image.h>
#include "../include/board.h"
#include "../include/rules.h"
#include "../include/renderer.h"
#include "../include/helperMethods.h"
#include "../include/movegen.h"
#include "../include/evaluate.h"

#define TOTAL_POSSIBLE_MOVES 218 // 218 possible moves
// Global running time variables
int running = 1;
int turnCounter = 0;
Piece* selectedPiece = NULL;
int selectedX = -1;
int selectedY = -1;
int mouseX = 0;
int mouseY = 0;
int isDragging = 0;
int turnStart = 1;
uint64_t* validMoves;
int playerColour = WHITE;
int aiOnly = 0;
// Renderer variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void endTurn();

void parseArgs(int argc, char* argv[]) {
    char* validArgs[] = {"-ai", "-b"};
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], validArgs[0]) == 0) {
                aiOnly = 1;
            } else if (strcmp(argv[i], validArgs[1]) == 0) {
                playerColour = BLACK;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    // Parse command line arguments
    parseArgs(argc, argv);
    // Allocate memory for the valid moves lookup table
    validMoves = malloc(sizeof(uint64_t) * TOTAL_POSSIBLE_MOVES);
    // Print the board (for debugging purposes)
    initRenderer(&window, &renderer);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    int colour;
    char* fenPtr = fen;

    int searchDepth = 3;

    parseFENToBitboard(fenPtr);
    // Initialise lookup tables
    initLookups();
    initPieceSqTables();

    // Save the initial board state
    // Setup the initial board state
    currentState.castleRights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;
    saveBoardState(&prevState);

    Position* validPositions = NULL;

    // Game loop
    while (running) {
        // Handle events
        // Exit if the ESC key is pressed
        if (turnStart) {
            turnCounter++;
            colour = !(turnCounter % 2);
            printf("Turn %d: %c to move.\n", turnCounter, turnCounter % 2 == 0 ? 'b' : 'w');

            getPseudoValidMoves(!colour, validMoves);
            // Clear the valid moves
            for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
                validMoves[i] = 0;
            }
            getPseudoValidMoves(colour, validMoves);
            saveBoardState(&prevState);

            validateMoves(colour, validMoves);

            // Count the number of valid moves
            int numValidMoves = countValidMoves(validMoves);

            printf("Number of valid moves: %d\n", numValidMoves);

            if (isCheck(colour)) {
                printf("Check!\n");
                if (numValidMoves == 0) {
                    printf("Checkmate! %c wins!\n", colour ? 'w' : 'b');
                    // turnStart = 0;
                    // Exit the game
                    running = 0;
                    break;
                }
            } else if (numValidMoves == 0) {
                printf("Stalemate!\n");
                running = 0;
                break; // Exit the game
            }

            // run minimax algorithm if it's the AI's turn
            if ((colour == !playerColour || aiOnly) && running) {
            // if (running) {
                if (numValidMoves == 1) {
                    // Forced move
                    printf("Forced move!\n");
                    searchDepth = 1;
                // } else if (numValidMoves <= 5) {
                //     searchDepth = 7;
                } else if (numValidMoves <= 30) {
                    searchDepth = 6;
                } else {
                    searchDepth = 5;
                }
                Move bestMove = findBestMove(4, colour, validMoves);
                if (bestMove.from == -1 || bestMove.to == -1) {
                    printf("No valid moves found! Probably checkmate, or there's a bug in the code.\n");
                    running = 0;
                    break;
                }

                printf("Best move: %c%d (%d) to %c%d (%d)\n", colToFile(bestMove.from % 8), 8 - bestMove.from / 8, bestMove.from, colToFile(bestMove.to % 8), 8 - bestMove.to / 8, bestMove.to);

                if (makeMove(bestMove, 0)) {
                    selectedPiece = NULL;
                    selectedX = -1;
                    selectedY = -1;
                    validPositions = NULL;
                    turnStart = 1;
                    // endTurn();
                }

            }

            if (playerColour == colour && !aiOnly) turnStart = 0;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            running = (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) ? 0 : running;

            uint64_t bitboard = currentState.bitboards[colour == WHITE ? WHITE_GLOB : BLACK_GLOB];

            // Handle mouse button down event
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                // Get the square the mouse is clicking on
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / SQUARE_SIZE;
                int col = x / SQUARE_SIZE;
                // Check if the mouse is clicking on a non-empty square of the correct color
                if (bitboard & (1ULL << (row * BOARD_SIZE + col))) {
                    // Set the selected piece
                    Piece tmp = getPieceAtSquare(row * BOARD_SIZE + col);
                    selectedPiece = &tmp;
                    printf("Selected piece: %c\n", pieceToChar(*selectedPiece));
                    selectedX = col;
                    selectedY = row;
                    isDragging = 1;
                    printValidMoves(validMoves[row * BOARD_SIZE + col]);
                }
            } 

            // Handle mouse motion event(s)
            if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
                int row = mouseY / SQUARE_SIZE;
                int col = mouseX / SQUARE_SIZE;
                // Check if the mouse is hovering over a non-empty square
                if (currentState.bitboards[colour == WHITE ? WHITE_GLOB : BLACK_GLOB] & (1ULL << (row * BOARD_SIZE + col)) && !isDragging) {
                    // Compute the valid moves for the piece
                    validPositions = bitboardToPosition(validMoves[row * BOARD_SIZE + col]);
                } else if (!isDragging) {
                    validPositions = NULL;
                }
            }

            // Handle mouse button up event
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && isDragging) {
                isDragging = 0;
                // Handle mouse button up
                int releaseX = mouseX / SQUARE_SIZE;
                int releaseY = mouseY / SQUARE_SIZE;

                int squareFrom = selectedY * BOARD_SIZE + selectedX;
                int squareTo = releaseY * BOARD_SIZE + releaseX;
                Move move = {*selectedPiece, squareFrom, squareTo};
                if (isMoveValid(move, validMoves)) {
                    printf("Move is valid!\n");
                    Move move = {*selectedPiece, squareFrom, squareTo};
                    int success = makeMove(move, 0);
                    if (success) {
                        // endTurn();
                        turnStart = 1;
                        selectedPiece = NULL;
                        selectedX = -1;
                        selectedY = -1;
                        validPositions = NULL;
                    }
                } else {
                    printf("Move is invalid!\n");
                }
            }
        }

        // Update game state
        renderFrame(&renderer, selectedX, selectedY, isDragging, validPositions);
    }

    // Cleanup and quit
    cleanupRenderer(&window, &renderer);
    // Free lookup tables
    free(validMoves);
    return 0;
}

void endTurn() {
    turnStart = 1;
    selectedPiece = NULL;
    selectedX = -1;
    selectedY = -1;
    validMoves = NULL;
}
