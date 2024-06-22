#ifndef ZOBRIST_H
#define ZOBRIST_H
#include <stdint.h>
#include <board.h>

void initZobrist();
uint64_t generateZobristKey(BoardState* state);

#endif // ZOBRIST_H