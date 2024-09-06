#include <stdint.h>
#include <board.h>
#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#define TABLE_SIZE 1000003  // Choose a prime number for table size
// #define TABLE_SIZE 6700417

typedef struct {
    uint64_t zobristKey;
    int depth;
    int value;
    Move bestMove;
    int flag;  // 0: exact, 1: alpha, 2: beta
    int color; // 0 for white, 1 for black (player to move when this entry was created)
} TTEntry;


extern TTEntry* transpositionTable;

void initTranspositionTable();

void freeTranspositionTable();

int probeTranspositionTable(uint64_t zobristKey, int depth, int alpha, int beta, int* value, Move* bestMove, int color);

void storeInTranspositionTable(uint64_t zobristKey, int depth, int value, Move bestMove, int flag, int color);

#endif // TRANSPOSITION_H