#include <stdio.h>
#include "../include/board.h"
#include "../include/helperMethods.h"
#define BOARD_SIZE 8

Piece board[BOARD_SIZE][BOARD_SIZE];
uint64_t bitboards[15];
uint64_t attackBitboards[15];
uint64_t pawnAttackLookup[2][64];
uint64_t knightAttackLookup[64];
uint64_t* rookAttackLookup[64];
uint64_t* bishopAttackLookup[64];
uint64_t* queenAttackLookup[64];
uint64_t kingAttackLookup[64];

PieceType getType(Position pos) {
    return board[pos.row][pos.col].type;
}

PieceColor getColor(Position pos) {
    return board[pos.row][pos.col].color;
}

char* getPieceSprite(PieceType type, PieceColor color) {
    return pieceSprites[(int)(type) - 1 + color * 6];
}

// Piece (*initBoard())[BOARD_SIZE][BOARD_SIZE] {
//     // Initialize the back rank for both white and black pieces
//     PieceType backRank[] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
//     for (int col = 0; col < BOARD_SIZE; col++) {
//         board[0][col] = (Piece){backRank[col], WHITE, pieceSprites[(int)(backRank[col]) - 1]};
//         board[7][col] = (Piece){backRank[col], BLACK, pieceSprites[((int)(backRank[col])) + 5]};
//     }

//     // Initialize the front rank for both white and black pawns
//     for (int col = 0; col < BOARD_SIZE; col++) {
//         board[1][col] = (Piece){PAWN, WHITE, pieceSprites[0]};
//         board[6][col] = (Piece){PAWN, BLACK, pieceSprites[6]};
//     }

//     // Initialize the rest of the board with empty squares
//     for (int row = 2; row < 6; row++) {
//         for (int col = 0; col < BOARD_SIZE; col++) {
//             board[row][col] = (Piece){EMPTY, WHITE, NULL};
//         }
//     }

//     return &board;
// }

Piece (*initBoard())[BOARD_SIZE][BOARD_SIZE] {
    for(int i=0; i<12; i++) {
        for (int j=0; j<BOARD_SIZE*BOARD_SIZE; j++) {
            int row = j / 8;
            int col = j % 8;
            if (bitboards[i+1] & (1ULL << j)) {
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