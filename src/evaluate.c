#include <board.h>
#include <evaluate.h>
#include <movegen.h>
#include <rules.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define INFINITY INT_MAX
struct PieceValues pieceValues = {
    .values = {100, 320, 330, 500, 900, 200000}
};

int evaluateMaterial(uint64_t* bitboards);
int max(int a, int b);
int min(int a, int b);

int minimax(int depth, int alpha, int beta, int color) {
    if (depth == 0 || isCheckmate(color) || isCheckmate(!color)) {
        int eval = evaluateMaterial(bitboards);
        // printf("Depth: %d, Eval: %d\n", depth, eval);
        return eval;
    }

    int breakFlag = 0;
    uint64_t* moves = malloc(sizeof(uint64_t) * BOARD_SIZE * BOARD_SIZE);

    uint64_t bitboardsCopy[15];
    uint64_t attackBitboardsCopy[15];
    uint8_t castleRightsCopy = castleRights;
    uint64_t enPassantMaskCopy = enPassantMask;

    uint64_t bitboardsSubCopy[15];
    uint64_t attackBitboardsSubCopy[15];
    uint8_t castleRightsSubCopy = castleRights;
    uint64_t enPassantMaskSubCopy = enPassantMask;
    // Save the current state of the board
    memcpy(bitboardsCopy, bitboards, sizeof(bitboards));
    memcpy(attackBitboardsCopy, attackBitboards, sizeof(attackBitboards));
    castleRightsCopy = castleRights;
    enPassantMaskCopy = enPassantMask;

    getPseudoValidMoves(color, moves);
    validateMoves(color, moves);

    int bestValue = color == WHITE ? -INFINITY : INFINITY;

    for (int pieceType = 1; pieceType < 7; pieceType++) {
        if (breakFlag) {
            break;
        }
        int bitboardIndex = getBitboardIndex(pieceType, color);
        uint64_t bitboard = bitboards[bitboardIndex];
        while (bitboard && !breakFlag) {
            int squareFrom = __builtin_ctzll(bitboard);
            uint64_t pieceMoves = moves[squareFrom];

            while (pieceMoves && !breakFlag) {
                int squareTo = __builtin_ctzll(pieceMoves);
                // Save the current state of the board
                memcpy(bitboardsSubCopy, bitboards, sizeof(bitboards));
                memcpy(attackBitboardsSubCopy, attackBitboards, sizeof(attackBitboards));
                castleRightsSubCopy = castleRights;
                enPassantMaskSubCopy = enPassantMask;
                makeMove((Piece){pieceType, color}, squareFrom, squareTo, moves, 0);

                int score = minimax(depth - 1, alpha, beta, color);

                // Restore the board state
                memcpy(bitboards, bitboardsSubCopy, sizeof(bitboards));
                memcpy(attackBitboards, attackBitboardsSubCopy, sizeof(attackBitboards));
                castleRights = castleRightsSubCopy;
                enPassantMask = enPassantMaskSubCopy;
                // memcpy(bitboards, bitboardsCopy, sizeof(bitboards));
                // memcpy(attackBitboards, attackBitboardsCopy, sizeof(attackBitboards));
                // castleRights = castleRightsCopy;
                // enPassantMask = enPassantMaskCopy;

                // White is maximizing player, black is minimizing player
                if (color == WHITE) {
                    bestValue = max(bestValue, score);
                    alpha = max(alpha, score);
                    if (beta <= alpha) {
                        free(moves);
                        breakFlag = 1;
                        break; // beta cut-off
                    }
                } else {
                    bestValue = min(bestValue, score);
                    beta = min(beta, score);
                    if (beta <= alpha) {
                        free(moves);
                        breakFlag = 1;
                        break; // alpha cut-off
                    }
                }
                pieceMoves &= ~(1ULL << squareTo);
            }
            bitboard &= ~(1ULL << squareFrom);
        }
    }

    free(moves);
    // Restore the board state
    memcpy(bitboards, bitboardsCopy, sizeof(bitboards));
    memcpy(attackBitboards, attackBitboardsCopy, sizeof(attackBitboards));
    castleRights = castleRightsCopy;
    enPassantMask = enPassantMaskCopy;
    // printf("Returning alpha: %d at depth %d\n", alpha, depth);
    // printf("Bitboard check:\n");
    // prettyPrintBitboard(bitboards[0]);
    return alpha;
}


Move findBestMove(int depth, int color, uint64_t* moves) {
    // uint64_t* moves = malloc(sizeof(uint64_t) * BOARD_SIZE * BOARD_SIZE);
    uint64_t bitboardsCopy[15];
    uint64_t attackBitboardsCopy[15];
    uint8_t castleRightsCopy = castleRights;
    uint64_t enPassantMaskCopy = enPassantMask;
    // Save the current state of the board
    memcpy(bitboardsCopy, bitboards, sizeof(bitboards));
    memcpy(attackBitboardsCopy, attackBitboards, sizeof(attackBitboards));
    castleRightsCopy = castleRights;
    enPassantMaskCopy = enPassantMask;
    printf("Bitboard check:\n");
    prettyPrintBitboard(bitboards[0]);

    // getPseudoValidMoves(color, moves);
    // validateMoves(color, moves);

    Move bestMove = {-1, -1};

    int bestValue = color == WHITE ? -INFINITY : INFINITY;

    for (int pieceType = 1; pieceType < 7; pieceType++) {
        int bitboardIndex = getBitboardIndex(pieceType, color);
        uint64_t bitboard = bitboards[bitboardIndex];
        while (bitboard) {
            int squareFrom = __builtin_ctzll(bitboard);
            uint64_t pieceMoves = moves[squareFrom];

            while (pieceMoves) {
                int squareTo = __builtin_ctzll(pieceMoves);
                makeMove((Piece){pieceType, color}, squareFrom, squareTo, moves, 0);

                int score = minimax(depth - 1, -INFINITY, INFINITY, !color);

                // Restore the board state
                memcpy(bitboards, bitboardsCopy, sizeof(bitboards));
                memcpy(attackBitboards, attackBitboardsCopy, sizeof(attackBitboards));
                castleRights = castleRightsCopy;
                enPassantMask = enPassantMaskCopy;

                if (color == WHITE) {
                    if (score > bestValue) {
                        bestValue = score;
                        bestMove = (Move){(Piece){pieceType, color}, squareFrom, squareTo};
                    }
                } else {
                    if (score < bestValue) {
                        bestValue = score;
                        bestMove = (Move){(Piece){pieceType, color}, squareFrom, squareTo};
                    }
                }
                pieceMoves &= ~(1ULL << squareTo);
            }
            bitboard &= ~(1ULL << squareFrom);
        }
    }
    // free(moves);
    // Restore the board state
    memcpy(bitboards, bitboardsCopy, sizeof(bitboards));
    memcpy(attackBitboards, attackBitboardsCopy, sizeof(attackBitboards));
    castleRights = castleRightsCopy;
    enPassantMask = enPassantMaskCopy;
    return bestMove;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}


int evaluateMaterial(uint64_t* bitboards) {
    // Evaluate the material on the board
    int score = 0;
    for (int pieceType = 0; pieceType < 6; pieceType++) {
        int whiteIndex = pieceType;
        int blackIndex = pieceType + 6;

        score += __builtin_popcountll(bitboards[whiteIndex]) * pieceValues.values[pieceType];
        score -= __builtin_popcountll(bitboards[blackIndex]) * pieceValues.values[pieceType];
    }

    return score;
}
