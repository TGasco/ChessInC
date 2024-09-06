#ifndef EVALUATE_H
#define EVALUATE_H

// Define piece values
struct PieceValues {
    int values[6];
};

extern struct PieceValues pieceValues;

int negamax(int depth, int alpha, int beta, int color, int pov);
// Move findBestMove(int depth, int color);
// Move findBestMove(int depth, int color, uint64_t* moves);
Move findBestMove(int depth, int color, Move* moves);
int compareMoves(const void* a, const void* b);
void initPieceSqTables();

extern int midValue[6];
extern int endValue[6];

extern int midTable[12][64];
extern int endTable[12][64];

#endif // EVALUATE_H