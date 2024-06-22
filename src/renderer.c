#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "../include/board.h"
#include "../include/renderer.h"

// Colors
SDL_Color lightColor = {240, 217, 181, 255};
SDL_Color darkColor = {181, 136, 99, 255};

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

void renderPieceAtMouse(SDL_Renderer* renderer, Piece piece) {
    // Render a piece at the current mouse position
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    renderPiece(renderer, piece, mouseX - SQUARE_SIZE / 2, mouseY - SQUARE_SIZE / 2);
}

void renderHighlight(SDL_Renderer* renderer, int x, int y) {
    // Render a highlight on the board
    SDL_Color highlightYellow = {255, 255, 0, 64};

    SDL_Rect highlightRect = {x, y, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(renderer, highlightYellow.r, highlightYellow.g, highlightYellow.b, highlightYellow.a);
    SDL_RenderFillRect(renderer, &highlightRect);
}

// New renderFrame uses bitboards directly, no need to pass the board array
void renderFrame(SDL_Renderer** renderer, int selectedX, int selectedY, int isDragging, int numValidMoves, Move* validMoves) {
    // Render the chess board
    renderBoard(*renderer);
    // // Render the valid moves - loop through the array of valid moves and render a highlight on each square
    int move;
    for (move = 0; move < numValidMoves; move++) {
        if (validMoves[move].to == -1) {
            break;
        }
        int row = validMoves[move].to / BOARD_SIZE;
        int col = validMoves[move].to % BOARD_SIZE;
        renderHighlight(*renderer, col * SQUARE_SIZE, row * SQUARE_SIZE);
    }
    // if (validMoves != NULL) {
    //     int numValidMoves = 0;
    //     for (int i = 0; i < 28; ++i) {
    //         if (validMoves[i].row == -1 && validMoves[i].col == -1) {
    //             break;
    //         }
    //         renderHighlight(*renderer, validMoves[i].col * SQUARE_SIZE, validMoves[i].row * SQUARE_SIZE);
    //         numValidMoves++;
    //     }
    // }

    // Iterate over bitboards and render the pieces
    for (int pieceType = 1; pieceType < 13; pieceType++)
    {
        Piece piece = {pieceType % 6, pieceType < 7 ? WHITE : BLACK, pieceSprites[pieceType - 1]};
        uint64_t bitboard = currentState->bitboards[pieceType];
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            bitboard &= bitboard - 1;
            int row = square / BOARD_SIZE;
            int col = square % BOARD_SIZE;
            if (!(isDragging && row == selectedY && col == selectedX)) {
                renderPiece(*renderer, piece, col * SQUARE_SIZE, row * SQUARE_SIZE);
            }
            if (isDragging && row == selectedY && col == selectedX) {
                renderPieceAtMouse(*renderer, piece);
            }
        }
    }
    SDL_RenderPresent(*renderer);
}

int initRenderer(SDL_Window** window, SDL_Renderer** renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    // Initialize SDL_image (Unused for now)
    // if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    //     printf("IMG_Init Error: %s\n", IMG_GetError());
    //     return 1;
    // }

    // Create the window
    *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create the renderer
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}

void cleanupRenderer(SDL_Window** window, SDL_Renderer** renderer) {
    // Cleanup SDL
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    TTF_Quit();
    SDL_Quit();
}