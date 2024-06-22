#include <stdio.h>
#include "../include/board.h"
#include "../include/helperMethods.h"
#include <string.h>
#include <stack.h>
#define BOARD_SIZE 8

Piece board[BOARD_SIZE][BOARD_SIZE];

uint64_t pawnAttackLookup[2][64];
uint64_t knightAttackLookup[64];
uint64_t* rookAttackLookup[64];
uint64_t* bishopAttackLookup[64];
uint64_t* queenAttackLookup[64];
uint64_t kingAttackLookup[64];


// define the castling masks (castling moves for king and queenside, for white and black)
// Set bits show all square the King/Rook must move through to castle
uint64_t kingSideCastleMask[2] = {0x6000000000000000, 0x0000000000000060};
uint64_t queenSideCastleMask[2] = {0x0E00000000000000, 0x000000000000000E};
// uint8_t castleRights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;

uint64_t promotionMask[2] = {0x00000000000000FF, 0xFF00000000000000};

BoardState *currentState;

BoardState prevState;

Stack *stateHistory;

PieceType getType(Position pos) {
    return board[pos.row][pos.col].type;
}

PieceColor getColor(Position pos) {
    return board[pos.row][pos.col].color;
}

char* getPieceSprite(PieceType type, PieceColor color) {
    return pieceSprites[(int)(type) - 1 + color * 6];
}

Piece (*initBoard())[BOARD_SIZE][BOARD_SIZE] {
    for(int i=0; i<12; i++) {
        for (int j=0; j<BOARD_SIZE*BOARD_SIZE; j++) {
            int row = j / 8;
            int col = j % 8;
            if (currentState->bitboards[i+1] & (1ULL << j)) {
                board[row][col] = (Piece){(i%6)+1, i < 6 ? WHITE : BLACK, getPieceSprite((i%6)+1, i < 6 ? WHITE : BLACK)};
            }
        }
    }
    return &board;
}

void printBoard() {
    // Print the board to the console
    printf("\n");
    for (int row = 0; row < BOARD_SIZE; row++) {
        printf("%d ", 8 - row);
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("%c ", pieceToChar(board[row][col]));
        }
        printf("\n");
    }
}

void prettyPrintBitboard(uint64_t bitboard) {
    // Print the bitboard to the console
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("%c ", (bitboard & (1ULL << (row * BOARD_SIZE + col))) ? '1' : '0');
        }
        printf("\n");
    }
}

void verticalFlip(uint64_t* bitboard) {
    // Flip the bitboard vertically about the center ranks
    *bitboard = __builtin_bswap64(*bitboard);
}

int getBoardAtIndex(int index, int colour) {
    // Returns the index for the bitboard containing the piece at the given index
    // printf("Searching for piece at index %d\n", index);
    for (int i=1; i<7; i++) {
        int pieceIDx = i + ((colour) * 6);
        if (currentState->bitboards[pieceIDx] & (1ULL << index)) {
            return pieceIDx;
        }
    }
    // Return 0 if no piece is found
    prettyPrintBitboard(currentState->bitboards[colour ? 13 : 14]);
    printf("WARN: No piece found at index %d\n", index);
    return 1;
}

Piece getPieceAtSquare(int square) {
    // Returns the piece at the given square
    int colour = 0;
    if (currentState->bitboards[BLACK_GLOB] & (1ULL << square)) {
        colour = 1;
    }
    int pieceIDx = getBoardAtIndex(square, colour);
    return (Piece){(pieceIDx - (colour * 6)) , colour, getPieceSprite(pieceIDx - (colour * 6), colour)};
}