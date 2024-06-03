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

int canCastleKingside[2] = {1, 1};
int canCastleQueenside[2] = {1, 1};
// Renderer variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void endTurn();

int main(int argc, char* argv[]) {
    // Allocate memory for the valid moves lookup table
    validMoves = malloc(sizeof(uint64_t) * BOARD_SIZE * BOARD_SIZE);
    // Print the board (for debugging purposes)
    initRenderer(&window, &renderer);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    int colour;
    char* fenPtr = fen;
    parseFENToBitboard(fenPtr);

    initBoardState();
    // Initialise lookup tables
    initLookups();

    Position* validPositions = NULL;

    // Game loop
    while (running) {
        // Handle events
        // Exit if the ESC key is pressed
        if (turnStart) {
            turnCounter++;
            colour = !(turnCounter % 2);
            printf("Turn %d: %c to move.\n", turnCounter, turnCounter % 2 == 0 ? 'b' : 'w');
            // Update the bitboards
            updateBitboards();
            // Update the board state

            getPseudoValidMoves(colour, validMoves);
            validateMoves(colour, validMoves);
            // updateBitboards();
            prettyPrintBitboard(bitboards[0]);
            printf("\n");

            // run minimax algorithm if it's the AI's turn
            if (colour == !playerColour) {
                Move bestMove = findBestMove(3, BLACK, validMoves);
                printf("Best move: %c%d to %c%d\n", colToFile(bestMove.from % 8), 8 - bestMove.from / 8, colToFile(bestMove.to % 8), 8 - bestMove.to / 8);
                makeMove((Piece){getPieceAtSquare(bestMove.from).type, colour}, bestMove.from, bestMove.to, validMoves, 0);
                turnStart = 1;
                selectedPiece = NULL;
                selectedX = -1;
                selectedY = -1;
                validPositions = NULL;
                // endTurn();
            }

            if (isCheck(colour)) {
                printf("Check!\n");
                if (isCheckmate(colour)) {
                    printf("Checkmate! %c wins!\n", colour ? 'w' : 'b');
                    // Exit the game
                    running = 0;
                }
            }
            if (playerColour == colour) turnStart = 0;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            running = (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) ? 0 : running;

            uint64_t bitboard = bitboards[colour ? 14 : 13];

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
                // printf("Hovering over square %c%d\n", colToFile(col), 8 - row);
                if (bitboards[colour ? 14 : 13] & (1ULL << (row * BOARD_SIZE + col)) && !isDragging) {
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
                    int success = makeMove(*selectedPiece, squareFrom, squareTo, validMoves, 0);
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
