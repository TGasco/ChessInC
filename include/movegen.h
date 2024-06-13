#ifndef MOVEGEN_H
#define MOVEGEN_H
void initLookups();
// uint64_t getValidMoves(int pieceType, int colour, int square, uint64_t occupancy);
void getPseudoValidMoves(int colour, uint64_t* moves);
uint64_t getPawnCaptureMask(uint64_t bitboard, int colour);
extern uint64_t rookMasks[64];
extern uint64_t bishopMasks[64];
#endif