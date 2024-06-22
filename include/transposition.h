#include <stdint.h>
#include <board.h>
#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

// #define TABLE_SIZE 1000003  // Choose a prime number for table size
#define TABLE_SIZE 6700417

typedef struct {
    uint64_t zobristKey;
    int depth;
    int value;
    Move bestMove;
    int flag;  // 0: exact, 1: alpha, 2: beta
} TTEntry;

extern TTEntry* transpositionTable;

void initTranspositionTable();

void freeTranspositionTable();

int probeTranspositionTable(uint64_t zobristKey, int depth, int alpha, int beta, int* value, Move* bestMove);

void storeInTranspositionTable(uint64_t zobristKey, int depth, int value, Move bestMove, int flag);

#endif // TRANSPOSITION_H