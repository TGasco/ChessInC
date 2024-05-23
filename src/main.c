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
#include "../include/magics.h"

// Global running time variables
int running = 1;
int turnCounter = 1;
Piece* selectedPiece = NULL;
int selectedX = -1;
int selectedY = -1;
int mouseX = 0;
int mouseY = 0;
int isDragging = 0;
int enPassant = -1;
int whiteKingMoved = 0;
int blackKingMoved = 0;
int whiteRookMoved[] = {0, 0};
int blackRookMoved[] = {0, 0};
int turnStart = 1;
Position whiteKingPos = (Position){7, 4};
Position blackKingPos = (Position){0, 4};
Position* checkMoves;
Position** validMovesLookup;
uint64_t* validMoves2;
// The maximum number of valid moves for any given board size is (BOARD_SIZE - 1) * 4
// Assuming conventional chess rules, this would be a Queen on an empty board, which can move in any direction.
int maxNumPositions = (BOARD_SIZE - 1) * 4;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;


// !!!!!!!!! DEPRECATED !!!!!!!!!
// void makeMove(Position* validMoves, int releaseX, int releaseY, Piece (*board)[BOARD_SIZE]) {
//     int isValidMove = 0;
//     int enPessantFlag = 0;
//     for (int i = 0; i < maxNumPositions; ++i) {
//         if (validMoves[i].row == releaseY && validMoves[i].col == releaseX) {
//             printf("It works :)\n");
//             isValidMove = 1;
//             // Handle pawn promotion
//             if (selectedPiece->type == PAWN && (releaseY == 0 || releaseY == 7) && board[releaseY][releaseX].type == EMPTY) {
//                 selectedPiece->type = QUEEN;
//                 selectedPiece->sprite = pieceSprites[4 + selectedPiece->color * 6];
//                 // Set bitboard to empty
//                 bitboards[selectedPiece->type + (selectedPiece->color * 6)] &= ~(1ULL << (selectedY * BOARD_SIZE + selectedX));
//                 // Set new position in the bitboard to queen
//                 bitboards[QUEEN + (selectedPiece->color * 6)] |= (1ULL << (releaseY * BOARD_SIZE + releaseX));
//             }
//             if (selectedPiece->type == PAWN && (releaseY == selectedY + 2 || releaseY == selectedY - 2)) {
//                 enPassant = releaseX;
//                 enPessantFlag = 1;
//                 printf("En pessant opportunity on %c file\n", colToFile(enPassant));
//             }

//             // Handle en passant
//             if (selectedPiece->type == PAWN && releaseX != selectedX && board[releaseY][releaseX].type == EMPTY) {
//                 board[selectedY][releaseX] = (Piece){EMPTY, WHITE};
//                 // Set bitboard to empty
//                 bitboards[selectedPiece->type + (selectedPiece->color * 6)] &= ~(1ULL << (selectedY * BOARD_SIZE + releaseX));
//             }

//             if (selectedPiece->type == KING) {
//                 Position pos = (Position){releaseY, releaseX};
//                 if (selectedPiece->color == WHITE) {
//                     blackKingMoved = 1;
//                     blackKingPos = pos;
//                 } else {
//                     whiteKingMoved = 1;
//                     whiteKingPos = pos;
//                 }
//                 // Check if the king is castling
//                 if (releaseX == selectedX + 2) {
//                     // King side castle
//                     board[releaseY][releaseX - 1] = board[releaseY][releaseX + 1];
//                     board[releaseY][releaseX + 1] = (Piece){EMPTY, WHITE};
//                     // Set new position in the bitboard
//                     bitboards[selectedPiece->type + (selectedPiece->color * 6)] |= (1ULL << (releaseY * BOARD_SIZE + releaseX - 1));
//                     // Set old position in the bitboard to empty
//                     bitboards[selectedPiece->type + (selectedPiece->color * 6)] &= ~(1ULL << (releaseY * BOARD_SIZE + releaseX + 1));
//                 } else if (releaseX == selectedX - 2) {
//                     // Queen side castle
//                     board[releaseY][releaseX + 1] = board[releaseY][releaseX - 2];
//                     board[releaseY][releaseX - 2] = (Piece){EMPTY, WHITE};
//                     // Set new position in the bitboard
//                     bitboards[selectedPiece->type + (selectedPiece->color * 6)] |= (1ULL << (releaseY * BOARD_SIZE + releaseX + 1));
//                     // Set old position in the bitboard to empty
//                     bitboards[selectedPiece->type + (selectedPiece->color * 6)] &= ~(1ULL << (releaseY * BOARD_SIZE + releaseX - 2));
//                 }
//             } else if (selectedPiece->type == ROOK) {
//                 if (selectedPiece->color == WHITE) {
//                     if (selectedX == 0) {
//                         blackRookMoved[0] = 1;
//                     } else if (selectedX == 7) {
//                         blackRookMoved[1] = 1;
//                     }
//                 } else {
//                     if (selectedX == 0) {
//                         whiteRookMoved[0] = 1;
//                     } else if (selectedX == 7) {
//                         whiteRookMoved[1] = 1;
//                     }
//                 }
//             }
//             moveToNotation(*selectedPiece, (Position){selectedY, selectedX}, (Position){releaseY, releaseX}, board[releaseY][releaseX].type != EMPTY);

//             board[releaseY][releaseX] = *selectedPiece;
//             board[selectedY][selectedX] = (Piece){EMPTY, WHITE};

//             int index = releaseY * BOARD_SIZE + releaseX;

//             // Clear original position in the bitboard
//             bitboards[selectedPiece->type + (selectedPiece->color * 6)] &= ~(1ULL << (selectedY * BOARD_SIZE + selectedX));
//             // Set new position in the bitboard
//             bitboards[selectedPiece->type + (selectedPiece->color * 6)] |= (1ULL << (releaseY * BOARD_SIZE + releaseX));

//             // Check if two-step pawn move was made, set en passant flag if so
//             // Increment the turn counter
//             turnCounter++;
//             turnStart = 1;
//             // Print the move in algebraic notation
//             break;
//         } else if (validMoves[i].row == -1 && validMoves[i].col == -1) {
//             break;
//         }
//     }
//     if (enPassant != -1 && !enPessantFlag) {
//         enPassant = -1;
//     }
// }

void makeMove(Piece selectedPiece, Position* currPos, Position* destPos, uint64_t* validMoves) {
    // printf("Selected piece: %c\n", pieceToChar(selectedPiece));
    int currIndex = currPos->row * BOARD_SIZE + currPos->col;
    int destIndex = destPos->row * BOARD_SIZE + destPos->col;
    int bitboardIndex = getBitboardIndex(selectedPiece.type, selectedPiece.color);
    // Check if the move is valid
    // printf the attack bitboard
    if (!(validMoves[currIndex] & (1ULL << destIndex))) {
        printf("Invalid move!\n");
        return;
    }
    // Passed the validity check, so make the move
    // Check if the move is a capture
    if (bitboards[0] & (1ULL << destIndex)) {
        // Capture
        printf("Capture!\n");
        // Get the piece at the destination
        for (int i=1; i<7; i++) {
            int index = i + (!(selectedPiece.color) * 6);
            if (bitboards[index] & (1ULL << destIndex)) {
                // Found the piece
                // Remove the piece from the bitboard
                bitboards[index] &= ~(1ULL << destIndex);
                break;
            } else { continue; }
        }
    }

    // Move the piece
    // Clear the original position in the bitboard
    bitboards[bitboardIndex] &= ~(1ULL << currIndex);
    // Set the new position in the bitboard
    bitboards[bitboardIndex] |= (1ULL << destIndex);

    // Increment the turn counter
    turnCounter++;
    turnStart = 1;
}

int main(int argc, char* argv[]) {
    // Allocate memory for the valid moves lookup table
    validMovesLookup = malloc(sizeof(Position*) * BOARD_SIZE * BOARD_SIZE);
    validMoves2 = malloc(sizeof(uint64_t) * BOARD_SIZE * BOARD_SIZE);
    // Print the board (for debugging purposes)
    // printBoard();
    initRenderer(&window, &renderer);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    // char fen[] = "8/8/8/8/r7/8/7R/8";
    // char fen[] = "r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1";
    // char fen[] = "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8";
    int colour;
    char* fenPtr = fen;
    parseFENToBitboard(fenPtr);

    // Initialise lookup tables
    // initAllLookups();
    initLookups();

    // Initialize the board
    Piece (*board)[BOARD_SIZE] = initBoard();

    Position* validMoves = NULL;

    // Game loop
    while (running) {
        // Handle events
        // Exit if the ESC key is pressed
        if (turnStart) {
            colour = !(turnCounter % 2);
            printf("Turn %d: %c to move.\n", turnCounter, turnCounter % 2 == 0 ? 'b' : 'w');
            if (colour == 0 ? whiteKingMoved : blackKingMoved) {
                printf("King has moved!\n");
            }
            // Update the bitboards
            updateBitboards();
            prettyPrintBitboard(bitboards[0]);

            // Update the GUI board
            board = initBoard();

            // Compute all valid moves
            // computeAllValidMoves(board, §, colour, enPassant, 
            // colour == 0 ? blackKingMoved : whiteKingMoved, colour == 0 ? blackRookMoved : whiteRookMoved,
            //  colour == 0 ? blackKingPos : whiteKingPos);
            // computeMovesBitboard(colour);
            computeAllMoves(colour, validMoves2);

            //  Print the valid moves
            printf("Valid moves:\n");
            prettyPrintBitboard(attackBitboards[0]);

            // if (isCheckmate(validMovesLookup)) {
            //     printf("Checkmate!\n");
            //     // running = 0;
            // }
            turnStart = 0;
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
                // prettyPrintBitboard(bitboard);
                if (bitboard & (1ULL << (row * BOARD_SIZE + col))) {
                    // Compute the valid moves for the piece
                    Position pos = {row, col};
                    int kingMoved = turnCounter % 2 == 0 ? whiteKingMoved : blackKingMoved;
                    int* rookMoved = turnCounter % 2 == 0 ? whiteRookMoved : blackRookMoved;
                    // Get valid moves from the lookup table
                    // validMoves = validMovesLookup[row * BOARD_SIZE + col];
                    // Set the selected piece
                    printf("Hello world!\n");
                    selectedPiece = &board[row][col];
                    printf("Selected piece: %c\n", pieceToChar(*selectedPiece));
                    // selectedPiece = &(bitboards[0] & (1ULL << (row * BOARD_SIZE + col)));
                    selectedX = col;
                    selectedY = row;
                    isDragging = 1;
                }
                // if (board[row][col].type != EMPTY && turnCounter % 2 == board[row][col].color) {
                // } else {
                //     printf("Invalid move!\n");
                // }
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
                    Position pos = {row, col};
                    int kingMoved = turnCounter % 2 == 0 ? whiteKingMoved : blackKingMoved;
                    int* rookMoved = turnCounter % 2 == 0 ? whiteRookMoved : blackRookMoved;
                    // Get valid moves from the lookup table
                    // if (board[row][col].type == KNIGHT) {
                    //     validMoves = bitboardToPosition(knightAttackLookup[row * BOARD_SIZE + col]);
                    //     prettyPrintBitboard(knightAttackLookup[row * BOARD_SIZE + col]);
                    // } else {
                    //     validMoves = validMovesLookup[row * BOARD_SIZE + col];
                    // }
                    // uint64_t lookup = getLookupTable(board[row][col].type, board[row][col].color)[row * BOARD_SIZE + col];
                    validMoves = bitboardToPosition(validMoves2[row * BOARD_SIZE + col]);
                    // Print the valid moves
                    printf("Valid moves:\n");
                    prettyPrintBitboard(validMoves2[row * BOARD_SIZE + col]);
                } else if (!isDragging) {
                    validMoves = NULL;
                }
            }

            // Handle mouse button up event
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && isDragging) {
                isDragging = 0;
                // Handle mouse button up
                int releaseX = mouseX / SQUARE_SIZE;
                int releaseY = mouseY / SQUARE_SIZE;
                Position currPos = {selectedY, selectedX};
                Position destPos = {releaseY, releaseX};
                makeMove(*selectedPiece, &currPos, &destPos, validMoves2);
                // Reset dragging state
                selectedPiece = NULL;
                selectedX = -1;
                selectedY = -1;
                validMoves = NULL;
            }
        }

        // Update game state
        renderFrame(&renderer, board, selectedX, selectedY, isDragging, validMoves);
    }

    // Cleanup and quit
    cleanupRenderer(&window, &renderer);
    // Free lookup tables
    free(validMovesLookup);
    return 0;
}
