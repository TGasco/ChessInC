#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "../include/helperMethods.h"
#include "../include/board.h"
#include "../include/magicmoves.h"
#include "../include/rules.h"
#include <evaluate.h>

// Declare functions
void initLookups();
bool isAttacked(uint64_t bitboard, int colour);
int getPieceValue (int pieceType);
int getMoveScore(Move move);
uint64_t getPinnedPieces(int colour);
int getDirection(int squareA, int squareB);

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

bool isSquareEmpty(uint64_t* board, int square) {
    return (*board & (1ULL << square)) == 0;
}

bool canPawnMoveTwoSteps(uint64_t* board, int square, bool isWhite) {
    if (isWhite) {
        // White pawn two-step check
        if (square >= 8 && square <= 15) { // Second rank
            return isSquareEmpty(board, square + 8);
        }
    } else {
        // Black pawn two-step check
        if (square >= 48 && square <= 55) { // Seventh rank
            return isSquareEmpty(board, square - 8);
        }
    }
    return false;
}

uint64_t generatePawnMoveOTF(int square, bool colour) {
    uint64_t pawnLookup = pawnAttackLookup[colour][square];

    // Check if the pawn can move two steps
    if (!canPawnMoveTwoSteps(&currentState->bitboards[0], square, colour)) {
        // Remove the two-step move from the pawn lookup
        pawnLookup &= ~((1ULL << (square + (colour ? 16 : -16))));
    }
    return ((pawnLookup & ~currentState->bitboards[0]) | (getPawnCaptureMask(1ULL << square, colour) & (currentState->bitboards[colour ? 13 : 14] | currentState->enPassantMask)));
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
        uint64_t bitboard = (1ULL << i);
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
            if ((currentState->castleRights & (color ? BLACK_KINGSIDE : WHITE_KINGSIDE)) == 0) return 0;
            // Check if the squares between the king and rook are empty and not attacked
            return ((kingSideCastleMask[color] & currentState->bitboards[0]) == 0) && (!isAttacked(kingSideCastleMask[color], color));
        case 1: // Queenside
            if ((currentState->castleRights & (color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE)) == 0) return 0;
            // Check if the squares between the king and rook are empty and not attacked
            return ((queenSideCastleMask[color] & currentState->bitboards[0]) == 0) && (!isAttacked(queenSideCastleMask[color], color));
        default:
            return 0;
    }
}

// TODO: Fix castling rights
uint64_t getPseudoValidMove(Piece piece, int square, int colour) {
    // Generate pseudo-legal moves for the given piece
    uint64_t moves = 0ULL;
    uint64_t bitboardNoKing = currentState->bitboards[0] & ~(currentState->bitboards[(getBitboardIndex(KING, !colour))]);
    switch(piece.type) {
        case PAWN:
            moves = generatePawnMoveOTF(square, piece.color);
            break;
        case KNIGHT:
            moves = knightAttackLookup[square];
            break;
        case BISHOP:
            moves = getBishopAttacks(square, bitboardNoKing);
            break;
        case ROOK:
            moves = getRookAttacks(square, bitboardNoKing);
            break;
        case QUEEN:
            moves = (getRookAttacks(square, bitboardNoKing) | getBishopAttacks(square, bitboardNoKing));
            break;
        case KING:
            moves = kingAttackLookup[square]; // Get normal king moves
            // Check if the king can castle
            int startSquare = colour == WHITE ? 4 : 60;
            // Check if the king can castle
            if (canCastle(piece.color, 0)) {
                // Castle kingside
                // printf("Can castle kingside\n");
                moves |= 1ULL << (square + 2);
            } else {
            }
            if (canCastle(piece.color, 1)) {
                // Castle queenside
                // printf("Can castle queenside\n");
                moves |= 1ULL << (square - 2);
            }
            // Remove moves that put the king in check
            moves &= ~currentState->attackBitboards[piece.color == WHITE ? BLACK_GLOB : WHITE_GLOB];
            break;
        default:
            printf("Invalid piece type: %d\n", piece.type);
            return 0ULL;
    }

    return moves;
}

void getPseudoValidMoves(int colour, Move* moves) {
    currentState->numValidMoves = 0; // Reset the number of valid moves
    int numValidMoves = 0;

    uint64_t pinnedPieces = getPinnedPieces(colour);
    int kingSquare = __builtin_ctzll(currentState->bitboards[getBitboardIndex(KING, colour)]);
    uint64_t xRayOrth = getRookAttacks(kingSquare, indexToBitboard(kingSquare));
    uint64_t xRayDiag = getBishopAttacks(kingSquare, indexToBitboard(kingSquare));
    uint64_t pieceRay = 0ULL;
    uint64_t validSquares = currentState->checkBitboard;
    uint64_t validSquares2 = currentState->checkBitboard;
    int check = isCheck(colour);

    for (int pieceType=1; pieceType<7; pieceType++) {
        // reset the attack bitboard
        currentState->attackBitboards[pieceType + (colour * 6)] = 0ULL;
        uint64_t pieceBitboard = currentState->bitboards[pieceType + (colour * 6)];
        while (pieceBitboard) {
            int square = __builtin_ctzll(pieceBitboard);
            validSquares = currentState->checkBitboard;

            // Check if the piece is pinned
            if (pinnedPieces & (1ULL << square)) {
                // Check if the piece is pinned along a diagonal
                if (xRayDiag & (1ULL << square)) {
                    // Get XRay for the piece (bishop)
                    // printf("Piece on square %d is pinned along a diagonal\n", square);
                    pieceRay = getBishopAttacks(square, indexToBitboard(square));
                    validSquares &= pieceRay & xRayDiag;
                } else if (xRayOrth & (1ULL << square)) {
                    // Get XRay for the piece (rook)
                    // printf("Piece on square %d is pinned along a file or rank\n", square);
                    pieceRay = getRookAttacks(square, indexToBitboard(square));
                    validSquares &= pieceRay & xRayOrth;
                } else {
                    // printf("ERR");
                }
            }

            uint64_t move = getPseudoValidMove((Piece){pieceType, colour}, square, colour);
            pieceBitboard &= pieceBitboard - 1;
            // Skip if move is not within the valid squares
            if (pieceType != KING) {
                move &= validSquares;
                if (move == 0ULL) {
                    continue;
                }
            }


            // Does the move attack the enemy king?
            if (move & currentState->bitboards[getBitboardIndex(KING, !colour)]) {
                // If checkBitboard is already set, the king is in double check
                if (validSquares2 != 0xFFFFFFFFFFFFFFFFULL) {
                    // Double check
                    currentState->doubleCheck = 1;
                    continue;
                }
                // Add piece to the check bitboard
                validSquares2 = indexToBitboard(square);

                // If piece is a slider, add the ray to the check bitboard
                if (pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN) {
                    int oppKingSquare = __builtin_ctzll(currentState->bitboards[getBitboardIndex(KING, !colour)]);
                    uint64_t oppRayDiag = getBishopAttacks(oppKingSquare, currentState->bitboards[0]);
                    uint64_t oppRayOrth = getRookAttacks(oppKingSquare, currentState->bitboards[0]);
                   
                    // Get check direction
                    if (oppRayDiag & (1ULL << square)) {
                        // Get XRay for the piece (bishop)
                        validSquares2 |= (move & oppRayDiag);
                    } else if (oppRayOrth & (1ULL << square)) {
                        // Get XRay for the piece (rook)
                        validSquares2 |= (move & oppRayOrth);
                    } else {
                        // printf("ERR");
                    }
                }
            }

            switch(pieceType) {
                case PAWN:
                    // Remove forward moves from the attack bitboard
                    currentState->attackBitboards[pieceType + (colour * 6)] |= getPawnCaptureMask(1ULL << square, colour);
                    break;
                default:
                    currentState->attackBitboards[pieceType + (colour * 6)] |= move;
                    break;
            }

            // Remove friendly fire
            move &= ~currentState->bitboards[colour ? 14 : 13];

            // Split move bitboards into individual moves (for move ordering)
            while (move) {
                int to = __builtin_ctzll(move);
                moves[numValidMoves] = (Move){{pieceType, colour, NULL}, square, to, 0};
                // Set the score of the move
                moves[numValidMoves].score = getMoveScore(moves[numValidMoves]);
                move &= move - 1;
                numValidMoves++;
            }
        }
    }
    updateBitboards();
    currentState->numValidMoves = numValidMoves;
    currentState->checkBitboard = validSquares2;
    // printf("Valid squares: \n");
    // prettyPrintBitboard(validSquares);
    // printf("\n");
}

uint64_t getPinnedPieces(int colour) {

    // TEMP: REGET queen attacks for the opposite colour, split into rook and bishop attacks
    // Loop over queen bitboard
    // For each queen, get the rook and bishop attacks
    uint64_t queenBitboard = currentState->bitboards[getBitboardIndex(QUEEN, !colour)];
    uint64_t queenAttacksOrth = 0ULL;
    uint64_t queenAttacksDiag = 0ULL;
    while (queenBitboard) {
        int square = __builtin_ctzll(queenBitboard);
        queenAttacksOrth |= getRookAttacks(square, currentState->bitboards[0]);
        queenAttacksDiag |= getBishopAttacks(square, currentState->bitboards[0]);
        queenBitboard &= queenBitboard - 1;
    }
    uint64_t orthAttacks = currentState->attackBitboards[getBitboardIndex(ROOK, !colour)] | queenAttacksOrth;
    uint64_t diagAttacks = currentState->attackBitboards[getBitboardIndex(BISHOP, !colour)] | queenAttacksDiag;
    int kingSquare = __builtin_ctzll(currentState->bitboards[getBitboardIndex(KING, colour)]);
    uint64_t xRayOrth = getRookAttacks(kingSquare, currentState->bitboards[0]);
    uint64_t xRayDiag = getBishopAttacks(kingSquare, currentState->bitboards[0]);
    uint64_t potentialPins = (((xRayOrth & orthAttacks)) | (xRayDiag & diagAttacks)) & currentState->bitboards[0];

    // Foreach potential pin, check if it is genuine
    return potentialPins;
}

int getDirection(int squareA, int squareB) {
    // Finds the direction from squareA to squareB
    // Returns -1 if the squares are not in the same row, column, or diagonal

    // Returns 0 if the squares are in the same row or column (orthogonal)
    // Returns 1 if the squares are in the same diagonal (diagonal)

    int aRow = squareA / 8;
    int aCol = squareA % 8;
    int bRow = squareB / 8;
    int bCol = squareB % 8;

    // Check if the squares are in the same row or column
    if (aRow == bRow || aCol == bCol) {
        return 0; // Orthogonal
    } else if (abs(aRow - bRow) == abs(aCol - bCol)) {
        return 1; // Diagonal
    } else {
        return -1; // Not in the same row, column, or diagonal
    }
    return -1;
}

int validateMove2(int colour, Move* move, uint64_t potentialPins) {
    uint64_t pinnedPieces = getPinnedPieces(colour);
    int check = isCheck(colour);

    // Cases:
    // 1: King is in check - remove all moves that don't either block the check, capture the checking piece, or move the king

    // 2. The piece is pinned - remove all moves that move the piece outside of the pin direction
    return 1;
}

int validateMove(int colour, Move* move) {
    // Make the move
    int bitboardIndex = getBitboardIndex(move->piece.type, colour);
    if (!makeMove(*move, 1)) {
        // Remove from the attack bitboard
        currentState->attackBitboards[bitboardIndex] &= ~(indexToBitboard(move->to));
        if (move->piece.type == PAWN) {
            // Remove pawn captures from the attack bitboard
            currentState->attackBitboards[bitboardIndex] &= ~(getPawnCaptureMask(indexToBitboard(move->from), colour));
        }

        // Remove the move from the valid moves
        *move = (Move){{0, 0, NULL}, -1, -1, -999999};
        return 0;
    }
    return 1;
}

void validateMoves(int colour, Move* validMoves) {
    // Valid moves are correct here
    // printf("Validating moves\n");
    int originalNumValidMoves = currentState->numValidMoves;
    int validMoveCount = 0;
    for (int move = 0; move < originalNumValidMoves; move++) {
        int bitboardIndex = getBitboardIndex(validMoves[move].piece.type, colour);
        if (validateMove(colour, &(validMoves[move]))) {
            validMoveCount++;
        }
    }
    currentState->numValidMoves = validMoveCount;
    updateBitboards();
}


int getMoveScore(Move move) {
    // Get the score of a move
    int score = 0;
    if (move.from == -1 || move.to == -1) {
        return -999999;
    }
    // Check if the move is a capture
    if (isCapture(indexToBitboard(move.to), move.piece.color, move.piece.type)) {
        // Score incentivises the AI to capture higher value pieces with lower value pieces
        score += getPieceValue(move.piece.type) - getPieceValue(PAWN);
        // score += getPieceValue(move.piece.type) - getPieceValue(getBoardAtIndex(move.to, !move.piece.color));
        if (score <= 0) {
            score = 81;
        }

    }
    if (isAttacked(indexToBitboard(move.to), move.piece.color)) {
        score -= getPieceValue(move.piece.type);
    }

    // Add scores for positional advantage versus its current position
    score += midTable[move.piece.type + (move.piece.color*6)-1][move.to] - midTable[move.piece.type + (move.piece.color*6)-1][move.from];
    // printf("Score: %d\n", score);
    return score;
}

// TEMP: Integrate w move values in evaluation
int getPieceValue (int pieceType) {
    // Get the value of a piece
    switch(pieceType) {
        case PAWN:
            return 82;
        case KNIGHT:
            return 337;
        case BISHOP:
            return 365;
        case ROOK:
            return 477;
        case QUEEN:
            return 1025;
        case KING:
            return 0;
        default:
            return 0;
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
    if (currentState->attackBitboards[colour ? 13 : 14] & bitboard) {
        return true;
    } else {
        return false;
    }
}