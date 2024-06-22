#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
// #include <SDL_image.h>
#include <board.h>
#include <rules.h>
#include <renderer.h>
#include <helperMethods.h>
#include <movegen.h>
#include <evaluate.h>
#include <zobrist.h>
#include <transposition.h>

#define MAX_DEPTH 25 // Maximum depth for the minimax algorithm
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
Move* validMoves;
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

void resetValidMoves() {
    for (int i = 0; i < TOTAL_POSSIBLE_MOVES; i++) {
        validMoves[i].from = -1;
        validMoves[i].to = -1;
        validMoves[i].score = -999999;
    }
}

int main(int argc, char* argv[]) {
    // Setup of game state
    // Parse command line arguments
    parseArgs(argc, argv);
    // Allocate memory for the valid moves lookup table
    validMoves = malloc(sizeof(Move) * TOTAL_POSSIBLE_MOVES);
    // Allocate memory for the current state pointer
    currentState = malloc(sizeof(BoardState));
    resetValidMoves();
    // Print the board (for debugging purposes)
    initRenderer(&window, &renderer);

    // Initialise the state history stack
    printf("Initialising state history stack.\n");
    stateHistory = createStack(sizeof(BoardState), 256);

    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    // char fen[] = "q2r1rk1/1p1b1ppp/p1nppn2/8/2P1P3/2N2N2/PP2BPPP/R2Q1RK1";
    int sideToMove;
    char* fenPtr = fen;

    parseFENToBitboard(fenPtr, currentState);
    // Initialise lookup tables
    printf("Initialising lookup tables.\n");
    initLookups();
    initPieceSqTables();
    updateBitboards();
    currentState->castleRights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;
    currentState->checkBitboard = 0xFFFFFFFFFFFFFFFFULL;
    currentState->doubleCheck = 0;
    currentState->halfMoveClock = 1;
    currentState->sideToMove = WHITE;
    // Generate the attack bitboards for the initial state
    printf("Generating attack bitboards.\n");
    getPseudoValidMoves(WHITE, validMoves);
    getPseudoValidMoves(BLACK, validMoves);
    resetValidMoves();
    // updateBitboards();

    // Initialise the Zobrist hash
    printf("Initialising Zobrist hash.\n");
    initZobrist();

    // Initialise the transposition table
    printf("Initialising transposition table.\n");
    initTranspositionTable();

    // Push the initial state onto the stack
    printf("Pushing initial state onto the stack.\n");
    push(stateHistory, currentState);

    // Game loop
    while (running) {
        // Handle events
        // Exit if the ESC key is pressed
        if (turnStart) {
            // Retrieve top of stack
            turnCounter++;
            sideToMove = !(turnCounter % 2);

            printf("Turn %d: %c to move.\n", currentState->halfMoveClock, currentState->sideToMove == 0 ? 'w' : 'b');
            getPseudoValidMoves(!sideToMove, validMoves);
            resetValidMoves();
            getPseudoValidMoves(sideToMove, validMoves);

            qsort(validMoves, currentState->numValidMoves, sizeof(Move), compareMoves);
            printf("Number of valid moves: %d\n", currentState->numValidMoves);
            printValidMoves(validMoves, TOTAL_POSSIBLE_MOVES);
            if (isCheck(sideToMove)) {
                printf("Check!\n");
                // Print the check bitboard
                prettyPrintBitboard(currentState->checkBitboard);
                if (currentState->numValidMoves == 0) {
                    printf("Checkmate!\n");
                    running = 0;
                    break; // Exit the game
                }
            } else if (currentState->numValidMoves == 0) {
                printf("Stalemate!\n");
                running = 0;
                break; // Exit the game
            }
            
            prettyPrintBitboard(currentState->bitboards[GLOBAL]);
            printf("Castling rights: %d\n", currentState->castleRights);

            // run minimax algorithm if it's the AI's turn
            if ((sideToMove == !playerColour || aiOnly) && running) {
                Move bestMove = findBestMove(MAX_DEPTH, sideToMove, validMoves);
                if (bestMove.from == -1 || bestMove.to == -1) {
                    printf("No valid moves found! Probably checkmate, or there's a bug in the code.\n");
                    running = 0;
                    break;
                }

                printf("Best move: %c%d (%d) to %c%d (%d) score: %d\n", colToFile(bestMove.from % 8), 8 - bestMove.from / 8, bestMove.from, colToFile(bestMove.to % 8), 8 - bestMove.to / 8, bestMove.to, bestMove.score);

                if (makeMove(bestMove, 0)) {
                    printf("Move successful!\n");
                    endTurn();
                } else {
                    printf("Move failed!\n");
                }
            } else {
            }

            if (playerColour == sideToMove && !aiOnly) turnStart = 0;
            // turnStart = 0;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            running = (event.type == SDL_QUIT || 
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) ? 0 : running;

            uint64_t bitboard = currentState->bitboards[sideToMove == WHITE ? WHITE_GLOB : BLACK_GLOB];

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
                    // printValidMoves(validMoves[row * BOARD_SIZE + col]);
                }
            } 

            // Handle mouse motion event(s)
            if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
                int row = mouseY / SQUARE_SIZE;
                int col = mouseX / SQUARE_SIZE;
                // Check if the mouse is hovering over a non-empty square
                if (currentState->bitboards[sideToMove == WHITE ? WHITE_GLOB : BLACK_GLOB] & (1ULL << (row * BOARD_SIZE + col)) && !isDragging) {
                    // Compute the valid moves for the piece
                    // validPositions = bitboardToPosition(validMoves[row * BOARD_SIZE + col]);
                } else if (!isDragging) {
                    // validPositions = NULL;
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
                    int success = makeMove(move, 0);
                    if (success) {
                        endTurn();
                    }
                } else {
                    printf("Move is invalid!\n");
                }
            }
        }

        // Update game state
        renderFrame(&renderer, selectedX, selectedY, isDragging, currentState->numValidMoves, validMoves);
    }

    // Cleanup and quit
    cleanupRenderer(&window, &renderer);
    // Free lookup tables
    free(validMoves);
    free(currentState);
    freeTranspositionTable();
    return 0;
}

void endTurn() {
    printf("Ending turn.\n");
    turnStart = 1;
    selectedPiece = NULL;
    selectedX = -1;
    selectedY = -1;

    // Get the current state from the stack
    currentState = peek(stateHistory);

    // Update the half move clock and side to move
    currentState->halfMoveClock++;
    currentState->sideToMove = (currentState->sideToMove == WHITE) ? BLACK : WHITE;
    // Reset check state
    // Reset the check state
    currentState->checkBitboard = 0xFFFFFFFFFFFFFFFFULL;
    currentState->doubleCheck = 0;
}
