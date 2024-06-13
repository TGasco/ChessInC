#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../include/board.h"
#include "../include/magicmoves.h"
// Declare functions
void initLookups();
bool isAttacked(uint64_t bitboard, int colour);

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

uint64_t getPawnCaptureMask(uint64_t bitboard, int colour) {
    uint64_t maskedBitboardL = bitboard & LEFT_WRAP_MASK;
    uint64_t maskedBitboardR = bitboard & RIGHT_WRAP_MASK;
    return colour ? (maskedBitboardL << 7) | (maskedBitboardR << 9) : (maskedBitboardL >> 9) | (maskedBitboardR >> 7);
}

bool isSquareEmpty(uint64_t board, int square) {
    return (board & (1ULL << square)) == 0;
}

bool canPawnMoveTwoSteps(uint64_t board, int square, bool isWhite) {
    if (isWhite) {
        // White pawn two-step check
        if (square >= 8 && square <= 15) { // Second rank
            return isSquareEmpty(board, square + 8) && isSquareEmpty(board, square + 16);
        }
    } else {
        // Black pawn two-step check
        if (square >= 48 && square <= 55) { // Seventh rank
            return isSquareEmpty(board, square - 8) && isSquareEmpty(board, square - 16);
        }
    }
    return false;
}

uint64_t generatePawnMoveOTF(int square, bool colour) {
    uint64_t pawnLookup = pawnAttackLookup[colour][square];

    // Check if the pawn can move two steps
    if (!canPawnMoveTwoSteps(currentState.bitboards[0], square, colour)) {
        // Remove the two-step move from the pawn lookup
        pawnLookup &= ~((1ULL << (square + (colour ? 16 : -16))));
    }
    return ((pawnLookup & ~currentState.bitboards[0]) | (getPawnCaptureMask(1ULL << square, colour) & (currentState.bitboards[colour ? 13 : 14] | currentState.enPassantMask)));
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
    // initMagics(); // Using precomputed magics for now
    init_magics();
    initSliderAttacks(bishop);
    initSliderAttacks(rook);
    initPawnLookup(); // Initialize the pawn lookup table
    initKnightLookup(); // Initialize the knight lookup table
    initKingLookup(); // Initialize the king lookup table
}


int canCastle(int color, int side) {
    switch(side) {
        case 0: // Kingside
            if ((currentState.castleRights & (color ? BLACK_KINGSIDE : WHITE_KINGSIDE)) == 0) return 0;
            // Check if the squares between the king and rook are empty and not attacked
            return ((kingSideCastleMask[color] & currentState.bitboards[0]) == 0) && (!isAttacked(kingSideCastleMask[color], color));
        case 1: // Queenside
            if ((currentState.castleRights & (color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE)) == 0) return 0;
            // Check if the squares between the king and rook are empty and not attacked
            return ((queenSideCastleMask[color] & currentState.bitboards[0]) == 0) && (!isAttacked(queenSideCastleMask[color], color));
        default:
            return 0;
    }
}

uint64_t getPseudoValidMove(Piece piece, int square) {
    // Generate pseudo-legal moves for the given piece
    uint64_t moves = 0ULL;

    switch(piece.type) {
        case PAWN:
            moves = generatePawnMoveOTF(square, piece.color);
            break;
        case KNIGHT:
            moves = knightAttackLookup[square];
            break;
        case BISHOP:
            moves = getBishopAttacks(square, currentState.bitboards[0]);
            break;
        case ROOK:
            moves = getRookAttacks(square, currentState.bitboards[0]);
            break;
        case QUEEN:
            moves = (getRookAttacks(square, currentState.bitboards[0]) | getBishopAttacks(square, currentState.bitboards[0]));
            break;
        case KING:
            moves = kingAttackLookup[square]; // Get normal king moves
            // Check if the king can castle
            if (canCastle(piece.color, 0)) {
                // Castle kingside
                moves |= 1ULL << (square + 2);
            }
            if (canCastle(piece.color, 1)) {
                // Castle queenside
                moves |= 1ULL << (square - 2);
            }
            break;
        default:
            printf("Invalid piece type: %d\n", piece.type);
            return 0ULL;
    }

    // Remove friendly fire
    moves &= ~currentState.bitboards[piece.color ? 14 : 13];

    return moves;
}

void getPseudoValidMoves(int colour, uint64_t* moves) {
    for (int pieceType=1; pieceType<7; pieceType++) {
        // reset the attack bitboard
        currentState.attackBitboards[pieceType + (colour * 6)] = 0ULL;
        uint64_t pieceBitboard = currentState.bitboards[pieceType + (colour * 6)];
        while (pieceBitboard) {
            int square = __builtin_ctzll(pieceBitboard);
            moves[square] = getPseudoValidMove((Piece){pieceType, colour}, square);
            pieceBitboard &= pieceBitboard - 1;
            switch(pieceType) {
                case PAWN:
                    // Remove forward moves from the attack bitboard
                    currentState.attackBitboards[pieceType + (colour * 6)] |= getPawnCaptureMask(1ULL << square, colour);
                    break;
                default:
                    currentState.attackBitboards[pieceType + (colour * 6)] |= moves[square];
                    break;
            }
        }
    }
}

// bool isAttacked(int square, int colour) {
//     if (attackBitboards[colour ? 13 : 14] & (1ULL << square)) {
//         return true;
//     } else {
//         return false;
//     }
// }

bool isAttacked(uint64_t bitboard, int colour) {
    if (currentState.attackBitboards[colour ? 13 : 14] & bitboard) {
        return true;
    } else {
        return false;
    }
}