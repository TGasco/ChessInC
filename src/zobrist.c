#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <board.h>

#define PIECE_TYPES 12
#define CASTLING_OPTIONS 4

uint64_t zobristTable[BOARD_SIZE*BOARD_SIZE][PIECE_TYPES];
uint64_t zobristCastling[CASTLING_OPTIONS];
uint64_t zobristEnPassant[BOARD_SIZE];
uint64_t zobristSideToMove;

void initZobrist() {
    srand(time(NULL));

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < PIECE_TYPES; j++) {
            zobristTable[i][j] = ((uint64_t)rand() << 32) | rand();
        }
    }

    for (int i = 0; i < CASTLING_OPTIONS; i++) {
        zobristCastling[i] = ((uint64_t)rand() << 32) | rand();
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        zobristEnPassant[i] = ((uint64_t)rand() << 32) | rand();
    }

    zobristSideToMove = ((uint64_t)rand() << 32) | rand();
}

int getPieceAt(int square) {
    // Mock function: should return the piece type at the given square
    // -1 indicates no piece
    // 0-5 for white pieces (P, N, B, R, Q, K), 6-11 for black pieces (p, n, b, r, q, k)
    // For demonstration purposes, return -1 (no piece)
    return -1;
}

uint64_t generateZobristKey(BoardState* state) {
    uint64_t hash = 0;

    // Loop through the bitboards and generate the hash
    for (int pieceType = 1; pieceType < 13; pieceType++) {
        uint64_t bitboard = state->bitboards[pieceType];
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            bitboard &= bitboard - 1;
            hash ^= zobristTable[square][pieceType - 1];
        }
    }

    if (state->castleRights & WHITE_KINGSIDE) {
        hash ^= zobristCastling[0];
    }
    if (state->castleRights & WHITE_QUEENSIDE) {
        hash ^= zobristCastling[1];
    }
    if (state->castleRights & BLACK_KINGSIDE) {
        hash ^= zobristCastling[2];
    }
    if (state->castleRights & BLACK_QUEENSIDE) {
        hash ^= zobristCastling[3];
    }

    if (state->enPassantMask != 0ULL) {
        // Find the file of the en passant square
        int file = __builtin_ctzll(state->enPassantMask) % 8;
        hash ^= zobristEnPassant[file];
    }

    if (state->sideToMove == 1) {
        hash ^= zobristSideToMove;
    }

    return hash;
}

// int main() {
//     initZobrist();
//     uint64_t key = generateZobristKey();
//     printf("Zobrist Key: %llu\n", key);
//     return 0;
// }
