#include <stdio.h>
#include "../include/board.h"
#include "../include/helperMethods.h"
#include <string.h>
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

BoardState currentState;

BoardState prevState;


// Piece tables for black and white are the same, but flipped vertically
int pawnPieceTable[2][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, -20, -20, 10, 10, 5,
        5, -5, -10, 0, 0, -10, -5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, 5, 10, 25, 25, 10, 5, 5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0, 0, 0, 0, 0, 0, 0, 0
    }
};

int knightPieceTable[2][64] = {
    {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    },
    {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    }
};

int bishopPieceTable[2][64] = {
    {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    },
    {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    }
};

int rookPieceTable[2][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 10, 10, 0, 0, 0
    },
    {
        0, 0, 0, 10, 10, 0, 0, 0,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        5, 10, 10, 10, 10, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    }
};

int queenPieceTable[2][64] = {
    {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    },
    {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -10, 5, 5, 5, 5, 5, 0, -10,
        0, 0, 5, 5, 5, 5, 0, -5,
        -5, 0, 5, 5, 5, 5, 0, -5,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    }
};

int kingPieceTable[2][64] = {
    {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
    },
    {
        20, 30, 10, 0, 0, 10, 30, 20,
        20, 20, 0, 0, 0, 0, 20, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30
    }
};

// int midPawnTable[64] = {
//       0,   0,   0,   0,   0,   0,  0,   0,
//      98, 134,  61,  95,  68, 126, 34, -11,
//      -6,   7,  26,  31,  65,  56, 25, -20,
//     -14,  13,   6,  21,  23,  12, 17, -23,
//     -27,  -2,  -5,  12,  17,   6, 10, -25,
//     -26,  -4,  -4, -10,   3,   3, 33, -12,
//     -35,  -1, -20, -23, -15,  24, 38, -22,
//       0,   0,   0,   0,   0,   0,  0,   0,
// };

// int endPawnTable[64] = {
//       0,   0,   0,   0,   0,   0,   0,   0,
//     178, 173, 158, 134, 147, 132, 165, 187,
//      94, 100,  85,  67,  56,  53,  82,  84,
//      32,  24,  13,   5,  -2,   4,  17,  17,
//      13,   9,  -3,  -7,  -7,  -8,   3,  -1,
//       4,   7,  -6,   1,   0,  -5,  -1,  -8,
//      13,   8,   8,  10,  13,   0,   2,  -7,
//       0,   0,   0,   0,   0,   0,   0,   0,
// };

// int midKnightTable[64] = {
//     -167, -89, -34, -49,  61, -97, -15, -107,
//      -73, -41,  72,  36,  23,  62,   7,  -17,
//      -47,  60,  37,  65,  84, 129,  73,   44,
//       -9,  17,  19,  53,  37,  69,  18,   22,
//      -13,   4,  16,  13,  28,  19,  21,   -8,
//      -23,  -9,  12,  10,  19,  17,  25,  -16,
//      -29, -53, -12,  -3,  -1,  18, -14,  -19,
//     -105, -21, -58, -33, -17, -28, -19,  -23,
// };

// int endKnightTable[64] = {
//     -58, -38, -13, -28, -31, -27, -63, -99,
//     -25,  -8, -25,  -2,  -9, -25, -24, -52,
//     -24, -20,  10,   9,  -1,  -9, -19, -41,
//     -17,   3,  22,  22,  22,  11,   8, -18,
//     -18,  -6,  16,  25,  16,  17,   4, -18,
//     -23,  -3,  -1,  15,  10,  -3, -20, -22,
//     -42, -20, -10,  -5,  -2, -20, -23, -44,
//     -29, -51, -23, -15, -22, -18, -50, -64,
// };

// int midBishopTable[64] = {
//     -29,   4, -82, -37, -25, -42,   7,  -8,
//     -26,  16, -18, -13,  30,  59,  18, -47,
//     -16,  37,  43,  40,  35,  50,  37,  -2,
//      -4,   5,  19,  50,  37,  37,   7,  -2,
//      -6,  13,  13,  26,  34,  12,  10,   4,
//       0,  15,  15,  15,  14,  27,  18,  10,
//       4,  15,  16,   0,   7,  21,  33,   1,
//     -33,  -3, -14, -21, -13, -12, -39, -21,
// };

// int endBishopTable[64] = {
//     -14, -21, -11,  -8, -7,  -9, -17, -24,
//      -8,  -4,   7, -12, -3, -13,  -4, -14,
//       2,  -8,   0,  -1, -2,   6,   0,   4,
//      -3,   9,  12,   9, 14,  10,   3,   2,
//      -6,   3,  13,  19,  7,  10,  -3,  -9,
//     -12,  -3,   8,  10, 13,   3,  -7, -15,
//     -14, -18,  -7,  -1,  4,  -9, -15, -27,
//     -23,  -9, -23,  -5, -9, -16,  -5, -17,
// };

// int midRookTable[64] = {
//      32,  42,  32,  51, 63,  9,  31,  43,
//      27,  32,  58,  62, 80, 67,  26,  44,
//      -5,  19,  26,  36, 17, 45,  61,  16,
//     -24, -11,   7,  26, 24, 35,  -8, -20,
//     -36, -26, -12,  -1,  9, -7,   6, -23,
//     -45, -25, -16, -17,  3,  0,  -5, -33,
//     -44, -16, -20,  -9, -1, 11,  -6, -71,
//     -19, -13,   1,  17, 16,  7, -37, -26,
// };

// int endRookTable[64] = {
//     13, 10, 18, 15, 12,  12,   8,   5,
//     11, 13, 13, 11, -3,   3,   8,   3,
//      7,  7,  7,  5,  4,  -3,  -5,  -3,
//      4,  3, 13,  1,  2,   1,  -1,   2,
//      3,  5,  8,  4, -5,  -6,  -8, -11,
//     -4,  0, -5, -1, -7, -12,  -8, -16,
//     -6, -6,  0,  2, -9,  -9, -11,  -3,
//     -9,  2,  3, -1, -5, -13,   4, -20,
// };

// int midQueenTable[64] = {
//     -28,   0,  29,  12,  59,  44,  43,  45,
//     -24, -39,  -5,   1, -16,  57,  28,  54,
//     -13, -17,   7,   8,  29,  56,  47,  57,
//     -27, -27, -16, -16,  -1,  17,  -2,   1,
//      -9, -26,  -9, -10,  -2,  -4,   3,  -3,
//     -14,   2, -11,  -2,  -5,   2,  14,   5,
//     -35,  -8,  11,   2,   8,  15,  -3,   1,
//      -1, -18,  -9,  10, -15, -25, -31, -50,
// };

// int endQueenTable[64] = {
//      -9,  22,  22,  27,  27,  19,  10,  20,
//     -17,  20,  32,  41,  58,  25,  30,   0,
//     -20,   6,   9,  49,  47,  35,  19,   9,
//       3,  22,  24,  45,  57,  40,  57,  36,
//     -18,  28,  19,  47,  31,  34,  39,  23,
//     -16, -27,  15,   6,   9,  17,  10,   5,
//     -22, -23, -30, -16, -16, -23, -36, -32,
//     -33, -28, -22, -43,  -5, -32, -20, -41,
// };

// int midKingTable[64] = {
//     -65,  23,  16, -15, -56, -34,   2,  13,
//      29,  -1, -20,  -7,  -8,  -4, -38, -29,
//      -9,  24,   2, -16, -20,   6,  22, -22,
//     -17, -20, -12, -27, -30, -25, -14, -36,
//     -49,  -1, -27, -39, -46, -44, -33, -51,
//     -14, -14, -22, -46, -44, -30, -15, -27,
//       1,   7,  -8, -64, -43, -16,   9,   8,
//     -15,  36,  12, -54,   8, -28,  24,  14,
// };

// int endKingTable[64] = {
//     -74, -35, -18, -18, -11,  15,   4, -17,
//     -12,  17,  14,  17,  17,  38,  23,  11,
//      10,  17,  23,  15,  20,  45,  44,  13,
//      -8,  22,  24,  27,  26,  33,  26,   3,
//     -18,  -4,  21,  24,  27,  23,   9, -11,
//     -19,  -3,  11,  21,  23,  16,   7,  -9,
//     -27, -11,   4,  13,  14,   4,  -5, -17,
//     -53, -34, -21, -11, -28, -14, -24, -43
// };

// int* midPestoTable[6] =
// {
//     midPawnTable,
//     midKnightTable,
//     midBishopTable,
//     midRookTable,
//     midQueenTable,
//     midKingTable
// };

// int* endPestoTable[6] =
// {
//     endPawnTable,
//     endKnightTable,
//     endBishopTable,
//     endRookTable,
//     endQueenTable,
//     endKingTable
// };

// int gamephaseInc[12] = {0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0};
// int midTable[12][64];
// int endTable[12][64];

// int midValue[6] = { 82, 337, 365, 477, 1025,  0};
// int endValue[6] = { 94, 281, 297, 512,  936,  0};

// void initPieceSqTables() {
//     for (int piece=PAWN; piece <= KING; piece++) {
//         for (int sq=0; sq<BOARD_SIZE*BOARD_SIZE; sq++) {
//             midTable[piece-1][sq] = midValue[piece-1] + midPestoTable[piece-1][sq];
//             endTable[piece-1][sq] = endValue[piece-1] + endPestoTable[piece-1][sq];

//             midTable[piece+5][sq] = midValue[piece-1] + midPestoTable[piece-1][FLIP(sq)];
//             endTable[piece+5][sq] = endValue[piece-1] + endPestoTable[piece-1][FLIP(sq)];
//         }
//     }
// }

// int taperedEval(uint64_t* bitboards) {
//     int mid[2];
//     int end[2];
//     int gamePhase = 0;
//     int maxGamePhase = 24;

//     // Evaluate all pieces for each colour
//     for (int colour=0; colour<2; colour++) {
//         for (int piece=1; piece<7; piece++) {
//             int pieceType = piece + (colour * 6) - 1;
//             for (int sq=0; sq<BOARD_SIZE*BOARD_SIZE; sq++) {
//                 if (bitboards[pieceType+1] & (1ULL << sq)) {
//                     mid[colour] += midTable[pieceType][sq];
//                     end[colour] += endTable[pieceType][sq];
//                     gamePhase += gamephaseInc[pieceType];
//                 }
//             }
//         }
//     }

//     int midScore = mid[WHITE] - mid[BLACK];
//     int endScore = end[WHITE] - end[BLACK];
//     int phase = gamePhase;
//     if (phase > maxGamePhase) phase = maxGamePhase;
//     int endPhase = maxGamePhase - phase;
//     return (((midScore * phase) + (endScore * endPhase)) / maxGamePhase);
// }


int** getPieceTable(int pieceType, int colour) {
    switch(pieceType) {
        case PAWN:
            return pawnPieceTable;
        case KNIGHT:
            return knightPieceTable;
        case BISHOP:
            return bishopPieceTable;
        case ROOK:
            return rookPieceTable;
        case QUEEN:
            return queenPieceTable;
        case KING:
            return kingPieceTable;
        default:
            return NULL;
    }
}

void saveBoardState(BoardState* state) {
    state->castleRights = currentState.castleRights;
    state->enPassantMask = currentState.enPassantMask;
    memcpy(state->bitboards, currentState.bitboards, sizeof(uint64_t) * 15);
    memcpy(state->attackBitboards, currentState.attackBitboards, sizeof(uint64_t) * 15);
}

void restoreBoardState(BoardState* state) {
    memcpy(currentState.bitboards, state->bitboards, sizeof(uint64_t) * 15);
    memcpy(currentState.attackBitboards, state->attackBitboards, sizeof(uint64_t) * 15);
    currentState.castleRights = state->castleRights;
    currentState.enPassantMask = state->enPassantMask;
}

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
            if (currentState.bitboards[i+1] & (1ULL << j)) {
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
    // prettyPrintBitboard(bitboards[colour ? 14 : 13]);
    for (int i=1; i<7; i++) {
        int pieceIDx = i + ((colour) * 6);
        if (currentState.bitboards[pieceIDx] & (1ULL << index)) {
            return pieceIDx;
        }
    }
    // Return 0 if no piece is found
    printf("WARN: No piece found at index %d\n", index);
    return -1;
}

Piece getPieceAtSquare(int square) {
    // Returns the piece at the given square
    int colour = 0;
    if (currentState.bitboards[BLACK_GLOB] & (1ULL << square)) {
        colour = 1;
    }
    int pieceIDx = getBoardAtIndex(square, colour);
    return (Piece){(pieceIDx - (colour * 6)) , colour, getPieceSprite(pieceIDx - (colour * 6), colour)};
}