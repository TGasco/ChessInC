#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/board.h"
#include "../include/magicmoves.h"
// Declare functions
void initLookups();


void initPawnLookup() {
    uint64_t pawnTwoStepMaskW = 0xFF00000000ULL;
    uint64_t pawnTwoStepMaskB = 0xFF000000ULL;
    // Iterate through the board
    for (int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) {
        // Initialize the bitboard to 0
        pawnAttackLookup[0][i] = 0ULL;
        pawnAttackLookup[1][i] = 0ULL;

        uint64_t bitboard = 1ULL << i;

        // Compute the pawn moves

        // 2-step pawn moves, only valid if the pawn is on the 2nd rank
        pawnAttackLookup[0][i] |= bitboard >> 16;
        pawnAttackLookup[1][i] |= bitboard << 16;
        // Apply mask to remove invalid 2-step pawn moves
        pawnAttackLookup[0][i] &= pawnTwoStepMaskW;
        pawnAttackLookup[1][i] &= pawnTwoStepMaskB;

        // For white pawns, shift up by 8 bits
        // For black pawns, shift down by 8 bits
        pawnAttackLookup[0][i] |= bitboard >> 8;
        pawnAttackLookup[1][i] |= bitboard << 8;
    }
}

void initKnightLookup() {
    int knightShift[] = {15, 17, 6, 10};
    uint64_t knightEdgeMaskL = 0xFCFCFCFCFCFCFCFCULL;
    uint64_t knightEdgeMaskR = 0x3F3F3F3F3F3F3F3FULL;
    uint64_t leftWrap = LEFT_WRAP_MASK;
    uint64_t rightWrap = RIGHT_WRAP_MASK;
    // Iterate through the board
    for (int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) {
        // Initialize the bitboard to 0
        knightAttackLookup[i] = 0ULL;
        uint64_t bitboard = 1ULL << i;

        // Compute the knight moves
        // The upright 'L' moves can be computed by shifting the bitboard up or down by 15 or 17 bits

        // Check if the knight is on the left edge of the board, ignore left 'L' moves
        uint64_t maskedBitboardL1 = bitboard & LEFT_WRAP_MASK;
        uint64_t maskedBitboardR1 = bitboard & RIGHT_WRAP_MASK;
        uint64_t maskedBitboardL2 = bitboard & knightEdgeMaskL;
        uint64_t maskedBitboardR2 = bitboard & knightEdgeMaskR;

        // The downright 'L' moves can be computed by shifting the bitboard up or down by 6 or 10 bits
        knightAttackLookup[i] |= maskedBitboardR2 >> 6;
        knightAttackLookup[i] |= maskedBitboardL2 << 6;

        knightAttackLookup[i] |= maskedBitboardL2 >> 10;
        knightAttackLookup[i] |= maskedBitboardR2 << 10;

        knightAttackLookup[i] |= maskedBitboardL1 << 15;
        knightAttackLookup[i] |= maskedBitboardR1 >> 15;

        knightAttackLookup[i] |= maskedBitboardR1 << 17;
        knightAttackLookup[i] |= maskedBitboardL1 >> 17;
    }
}

void initKingLookup() {
    // Iterate through the board
    for (int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) {
        // Initialize the bitboard to 0
        kingAttackLookup[i] = 0ULL;
        uint64_t bitboard = 1ULL << i;
        uint64_t maskedBitboardL = bitboard & LEFT_WRAP_MASK;
        uint64_t maskedBitboardR = bitboard & RIGHT_WRAP_MASK;

        // Compute the king moves
        kingAttackLookup[i] |= maskedBitboardL >> 1;
        kingAttackLookup[i] |= maskedBitboardR << 1;

        kingAttackLookup[i] |= maskedBitboardR >> 7;
        kingAttackLookup[i] |= maskedBitboardL << 7;

        kingAttackLookup[i] |= bitboard >> 8;
        kingAttackLookup[i] |= bitboard << 8;

        kingAttackLookup[i] |= maskedBitboardL >> 9;
        kingAttackLookup[i] |= maskedBitboardR << 9;
    }
}

void initLookups() {
    initMagics();
    initSliderAttacks(bishop);
    initSliderAttacks(rook);
    initPawnLookup(); // Initialize the pawn lookup table
    initKnightLookup(); // Initialize the knight lookup table
    initKingLookup(); // Initialize the king lookup table
}

// Assuming the board is represented as a 64-bit integer where each bit represents a square
uint64_t getValidMoves(int pieceType, int colour, int square, uint64_t occupancy) {
    uint64_t validMoves = 0;
    switch(pieceType) {
        case PAWN:
            switch (colour)
            {
            case WHITE:
                validMoves = pawnAttackLookup[0][square];
                break;
            case BLACK:
                validMoves = pawnAttackLookup[1][square];
                break;
            default:
                break;
            }
            break;
        case KNIGHT: // Knight
            validMoves = knightAttackLookup[square];
            break;
        case BISHOP: // Bishop
            validMoves = getBishopAttacks(square, occupancy);
            break;
        case ROOK: // Rook
            validMoves = getRookAttacks(square, occupancy);
            break;
        case QUEEN: // Queen
            // Get the occupancy used via reverse lookup
            validMoves = getRookAttacks(square, occupancy) | getBishopAttacks(square, occupancy);
            break;
        case KING: // King
            validMoves = kingAttackLookup[square];
            break;
        default:
            printf("Invalid piece type: %d\n", pieceType);
            break;
    }

    // Mask the valid moves with the occupancy bitboard to remove invalid moves
    validMoves &= ~bitboards[colour ? 14 : 13];

    return validMoves;
}