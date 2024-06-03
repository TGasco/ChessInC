#ifndef EVALUATE_H
#define EVALUATE_H

// Define piece values
struct PieceValues {
    int values[6];
};

extern struct PieceValues pieceValues;

int minimax(int depth, int alpha, int beta, int color);
// Move findBestMove(int depth, int color);
Move findBestMove(int depth, int color, uint64_t* moves);

#endif // EVALUATE_H