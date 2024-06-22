#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <transposition.h>

TTEntry* transpositionTable;

void initTranspositionTable() {
    transpositionTable = malloc(TABLE_SIZE * sizeof(TTEntry));
    if (transpositionTable == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        transpositionTable[i].zobristKey = 0;
    }
}

void freeTranspositionTable() {
    free(transpositionTable);
}

int probeTranspositionTable(uint64_t zobristKey, int depth, int alpha, int beta, int* value, Move* bestMove) {
    int index = zobristKey % TABLE_SIZE;
    TTEntry* entry = &transpositionTable[index];

    if (entry->zobristKey == zobristKey) {
        if (entry->depth >= depth) {
            if (entry->flag == 0) {  // Exact
                *value = entry->value;
                *bestMove = entry->bestMove;
                return 1;
            } else if (entry->flag == 1 && entry->value <= alpha) {  // Alpha
                *value = alpha;
                return 1;
            } else if (entry->flag == 2 && entry->value >= beta) {  // Beta
                *value = beta;
                return 1;
            }
        }
    }
    return 0;
}

void storeInTranspositionTable(uint64_t zobristKey, int depth, int value, Move bestMove, int flag) {
    int index = zobristKey % TABLE_SIZE;
    TTEntry* entry = &transpositionTable[index];

    entry->zobristKey = zobristKey;
    entry->depth = depth;
    entry->value = value;
    entry->bestMove = bestMove;
    entry->flag = flag;
}