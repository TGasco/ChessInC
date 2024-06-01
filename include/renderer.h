#include <SDL.h>
#include "../include/board.h"
#ifndef RENDERER_H
#define RENDERER_H

// Define path to assets

#define SQUARE_SIZE 50
#define WINDOW_WIDTH (BOARD_SIZE * SQUARE_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * SQUARE_SIZE)

#define ASSET_PATH "../assets/Roboto-Bold.ttf"

void renderBoard(SDL_Renderer* renderer);

void renderPiece(SDL_Renderer* renderer, Piece piece, int x, int y);

void renderPieceAtMouse(SDL_Renderer* renderer, Piece piece);

void renderHighlight(SDL_Renderer* renderer, int x, int y);

// void renderFrame(SDL_Renderer** renderer, Piece board[BOARD_SIZE][BOARD_SIZE], int selectedX, int selectedY, int isDragging, Position* validMoves);
void renderFrame(SDL_Renderer** renderer, int selectedX, int selectedY, int isDragging, Position* validMoves);

int initRenderer(SDL_Window** window, SDL_Renderer** renderer);

void cleanupRenderer(SDL_Window** window, SDL_Renderer** renderer);

#endif // RENDERER_H