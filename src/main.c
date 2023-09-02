#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
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
    // Set color and font size
    SDL_Color textColor = {0, 0, 0, 255}; // Black color for text
    TTF_Font* font = TTF_OpenFont(ASSET_PATH, 24); // Load a font of your choice
    char pieceStr[2] = { pieceToChar(piece), '\0' };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, pieceStr, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int width, height;
    SDL_QueryTexture(textTexture, NULL, NULL, &width, &height);
    
    SDL_Rect destRect = {x + SQUARE_SIZE / 2 - width / 2, y + SQUARE_SIZE / 2 - height / 2, width, height};

    SDL_RenderCopy(renderer, textTexture, NULL, &destRect);

    // Cleanup
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void renderHighlight(SDL_Renderer* renderer, int x, int y) {
    // Render a highlight on the board
    SDL_Color highlightYellow = {255, 255, 0, 64};

    SDL_Rect highlightRect = {x, y, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(renderer, highlightYellow.r, highlightYellow.g, highlightYellow.b, highlightYellow.a);
    SDL_RenderFillRect(renderer, &highlightRect);
}


int main(int argc, char* argv[]) {
    // Initialize the board
    Piece (*board)[BOARD_SIZE] = initBoard();
    // Print the board (for debugging purposes)
    printBoard();

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
        SDL_Event event;
        Position* validMoves;
        while (SDL_PollEvent(&event)) {
            running = (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) ? 0 : running;

            // Find the square the mouse is hovering over
            if (event.type == SDL_MOUSEMOTION) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / SQUARE_SIZE;
                int col = x / SQUARE_SIZE;
                // Check if the mouse is hovering over a non-empty square
                if (board[row][col].type != EMPTY) {
                    // printf("Mouse is hovering over piece %c at %d, %d\n", pieceToChar(board[row][col]), col, row);
                    // Compute the valid moves for the piece
                    Position pos = {row, col};
                    validMoves = computeValidMoves(pos, board[row][col], board);
                }
            }
        }
        // Update game state

        // Render the chess board
        renderBoard(renderer);
        
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                Piece piece = board[row][col];
                if (piece.type != EMPTY) {
                    renderPiece(renderer, piece, col * SQUARE_SIZE, row * SQUARE_SIZE);
                }
            }
        }

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

        SDL_RenderPresent(renderer);

    }

    // Cleanup and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
