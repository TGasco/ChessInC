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

    // Add the castling rights to the hash
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

    // Add the en passant square to the hash
    if (state->enPassantMask != 0ULL) {
        // Find the file of the en passant square
        int file = __builtin_ctzll(state->enPassantMask) % 8;
        hash ^= zobristEnPassant[file];
    }

    // Add the side to move to the hash
    if (state->sideToMove == BLACK) {
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
