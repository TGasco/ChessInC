#ifndef MAGICS_H
#define MAGICS_H
void initLookups();
uint64_t getValidMoves(int pieceType, int colour, int square, uint64_t occupancy);
extern uint64_t rookMasks[64];
extern uint64_t bishopMasks[64];

extern uint64_t rookMagics[64];
extern uint64_t bishopMagics[64];
#endif