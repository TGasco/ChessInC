#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../include/board.h"
#include "../include/rules.h"
// #include <SDL_image.h> // You might need this for loading images
// #define BOARD_SIZE 8
#define SQUARE_SIZE 50 // Adjust as needed
#define WINDOW_WIDTH (BOARD_SIZE * SQUARE_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * SQUARE_SIZE)
// Board representation, '-' is an empty square, 'P' is a white pawn, 'p' is a black pawn, etc.

// Define path to assets
#define ASSET_PATH "../assets/Roboto-Bold.ttf"

// Colors
SDL_Color lightColor = {240, 217, 181, 255};
SDL_Color darkColor = {181, 136, 99, 255};

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
Position* kingPos = NULL;
int check = 0;

void loadPieceSprites() {
    // Load the piece sprites

}

void renderBoard(SDL_Renderer* renderer) {
    // Render the chess board
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            SDL_Rect squareRect = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_SetRenderDrawColor(renderer, 
            (row + col) % 2 == 0 ? lightColor.r : darkColor.r, 
            (row + col) % 2 == 0 ? lightColor.g : darkColor.g, 
            (row + col) % 2 == 0 ? lightColor.b : darkColor.b, 255);
            SDL_RenderFillRect(renderer, &squareRect);
        }
    }
}

void renderPiece(SDL_Renderer* renderer, Piece piece, int x, int y) {
    // Render a piece on the board
    SDL_Texture* pieceTexture = IMG_LoadTexture(renderer, piece.sprite);
    SDL_Rect destRect = {x, y, SQUARE_SIZE, SQUARE_SIZE};
    SDL_RenderCopy(renderer, pieceTexture, NULL, &destRect);

    // Cleanup
    SDL_DestroyTexture(pieceTexture);
}

void renderHighlight(SDL_Renderer* renderer, int x, int y) {
    // Render a highlight on the board
    SDL_Color highlightYellow = {255, 255, 0, 64};

    SDL_Rect highlightRect = {x, y, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(renderer, highlightYellow.r, highlightYellow.g, highlightYellow.b, highlightYellow.a);
    SDL_RenderFillRect(renderer, &highlightRect);
}

void moveToNotation(Piece piece, Position posFrom, Position posTo, int capture) {
    // Convert a move to algebraic notation
    char notation[10];  // Assuming a reasonable maximum length for the notation

    if (piece.type != PAWN) {
        snprintf(notation, sizeof(notation), "%c", pieceToChar(piece));
    }

    if (capture) {
        if (piece.type == PAWN) {
            snprintf(notation, sizeof(notation), "%c", colToFile(posFrom.col));
        }
        strcat(notation, "x");
    }

    printf("%s%c%d\n", notation, colToFile(posTo.col), 8 - posTo.row);
}


void makeMove(Position* validMoves, int releaseX, int releaseY, Piece (*board)[BOARD_SIZE]) {
    int isValidMove = 0;
    int direction = (selectedPiece->color == WHITE) ? 1 : -1;
    for (int i = 0; i < 28; ++i) {
        if (validMoves[i].row == releaseY && validMoves[i].col == releaseX) {
            isValidMove = 1;
            // Handle pawn promotion
            if (selectedPiece->type == PAWN && (releaseY == 0 || releaseY == 7) && board[releaseY][releaseX].type == EMPTY) {
                selectedPiece->type = QUEEN;
            }
            if (selectedPiece->type == PAWN && (releaseY == selectedY + 2 || releaseY == selectedY - 2)) {
                enPassant = releaseX;
                printf("En pessant opportunity of %c file\n", colToFile(enPassant));
            }

            // Handle en passant
            if (selectedPiece->type == PAWN && releaseX == enPassant && (releaseY == selectedY + 1 || releaseY == selectedY - 1)) {
                board[selectedY][enPassant] = (Piece){EMPTY, WHITE};
                // Reset en passant flag
                enPassant = -1;
            }

            if (selectedPiece->type == KING) {
                if (selectedPiece->color == WHITE) {
                    whiteKingMoved = 1;
                } else {
                    blackKingMoved = 1;
                }
                // Check if the king is castling
                if (releaseX == selectedX + direction * 2) {
                    // King side castle
                    board[releaseY][releaseX - direction * 1] = board[releaseY][releaseX + direction * 1];
                    board[releaseY][releaseX + direction * 1] = (Piece){EMPTY, WHITE};
                } else if (releaseX == selectedX - direction * 2) {
                    // Queen side castle
                    board[releaseY][releaseX + direction * 1] = board[releaseY][releaseX - direction * 2];
                    board[releaseY][releaseX - direction * 2] = (Piece){EMPTY, WHITE};
                }
            } else if (selectedPiece->type == ROOK) {
                if (selectedPiece->color == WHITE) {
                    if (selectedX == 0) {
                        whiteRookMoved[0] = 1;
                    } else if (selectedX == 7) {
                        whiteRookMoved[1] = 1;
                    }
                } else {
                    if (selectedX == 0) {
                        blackRookMoved[0] = 1;
                    } else if (selectedX == 7) {
                        blackRookMoved[1] = 1;
                    }
                }
            }
            moveToNotation(*selectedPiece, (Position){selectedY, selectedX}, (Position){releaseY, releaseX}, board[releaseY][releaseX].type != EMPTY);

            board[releaseY][releaseX] = *selectedPiece;
            board[selectedY][selectedX] = (Piece){EMPTY, WHITE};

            // Find check (if exists)
            check = findCheck(board, turnCounter % 2 == 0 ? WHITE : BLACK, &kingPos);
            if (check) {
                printf("Check discovered!\n");
            }

            // Check if two-step pawn move was made, set en passant flag if so
            // Increment the turn counter
            turnCounter++;
            turnStart = 1;
            // Print the move in algebraic notation
            // printf("Turn %d. It is %c's turn...\n", turnCounter, turnCounter % 2 == 0 ? 'B' : 'W');
            break;
        }
    }
}


int main(int argc, char* argv[]) {
    // Initialize the board
    Piece (*board)[BOARD_SIZE] = initBoard();
    // Print the board (for debugging purposes)
    // printBoard();

    SDL_Init(SDL_INIT_VIDEO);
    // Additional initialization code

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        return 1;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create the renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }


    // Game loop
    while (running) {
        // Handle events
        // Exit if the ESC key is pressed
        if (turnStart) {
            printf("Turn %d: %c to move.\n", turnCounter, turnCounter % 2 == 0 ? 'b' : 'w');
            turnStart = 0;
        }

        SDL_Event event;
        Position* validMoves;
        while (SDL_PollEvent(&event)) {
            running = (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) ? 0 : running;
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                // Get the square the mouse is clicking on
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / SQUARE_SIZE;
                int col = x / SQUARE_SIZE;
                // Check if the mouse is clicking on a non-empty square
                if (board[row][col].type != EMPTY && turnCounter % 2 == board[row][col].color) {
                    // printf("Mouse is clicking on piece %c at %d, %d\n", pieceToChar(board[row][col]), col, row);
                    // Compute the valid moves for the piece
                    Position pos = {row, col};
                    int kingMoved = board[row][col].type == KING ? board[row][col].color == WHITE ? whiteKingMoved : blackKingMoved : 0;
                    int* rookMoved = board[row][col].type == ROOK ? board[row][col].color == WHITE ? whiteRookMoved : blackRookMoved : 0;
                    printf("Rook (a) moved: %d \t Rook (h) moved: %d\n", rookMoved[0], rookMoved[1]);
                    validMoves = computeValidMoves(pos, board[row][col], board, enPassant, kingMoved, rookMoved);
                    // Set the selected piece
                    selectedPiece = &board[row][col];
                    selectedX = col;
                    selectedY = row;
                    isDragging = 1;
                }
            } 
            // else {
            if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
                int row = mouseY / SQUARE_SIZE;
                int col = mouseX / SQUARE_SIZE;
                // Check if the mouse is hovering over a non-empty square
                if (board[row][col].type != EMPTY && turnCounter % 2 == board[row][col].color && !isDragging) {
                    // printf("Mouse is hovering over piece %c at %d, %d\n", pieceToChar(board[row][col]), col, row);
                    // Compute the valid moves for the piece
                    Position pos = {row, col};
                    int kingMoved = board[row][col].type == KING ? board[row][col].color == WHITE ? whiteKingMoved : blackKingMoved : 0;
                    int* rookMoved = board[row][col].type == ROOK ? board[row][col].color == WHITE ? whiteRookMoved : blackRookMoved : 0;
                    printf("Rook (a) moved: %d \t Rook (h) moved: %d\n", rookMoved[0], rookMoved[1]);
                    validMoves = computeValidMoves(pos, board[row][col], board, enPassant, kingMoved, rookMoved);
                } else if (!isDragging) {
                    validMoves = NULL;
                }
            }
            // }

            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && isDragging) {
                // Handle mouse button up
                int releaseX = mouseX / SQUARE_SIZE;
                int releaseY = mouseY / SQUARE_SIZE;
                makeMove(validMoves, releaseX, releaseY, board);
                // Reset dragging state
                isDragging = 0;
                selectedPiece = NULL;
                selectedX = -1;
                selectedY = -1;
            }
        }
        // Update game state

        // Render the chess board
        renderBoard(renderer);
        
        // Render the valid moves - loop through the array of valid moves and render a highlight on each square
        if (validMoves != NULL) {
            for (int i = 0; i < 28; ++i) {
                if (validMoves[i].row == 0 && validMoves[i].col == 0) {
                    break;
                }
                renderHighlight(renderer, validMoves[i].col * SQUARE_SIZE, validMoves[i].row * SQUARE_SIZE);
            }
        } else {
            // Remove the highlights if the mouse is not hovering over a piece
        }

        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                Piece piece = board[row][col];
                if (piece.type != EMPTY) {
                    renderPiece(renderer, piece, col * SQUARE_SIZE, row * SQUARE_SIZE);
                }
            }
        }


        SDL_RenderPresent(renderer);

    }

    // Cleanup and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
