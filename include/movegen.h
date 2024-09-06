#ifndef MOVEGEN_H
#define MOVEGEN_H
void initLookups();
// uint64_t getValidMoves(int pieceType, int colour, int square, uint64_t occupancy);
// void getPseudoValidMoves(int colour, uint64_t* moves);
void getPseudoValidMoves(int colour, Move* moves);
uint64_t getPseudoValidMove(Piece piece, int square, int colour, uint64_t occ);
int validateMove(int colour, Move* move);
void validateMoves(int colour, Move* validMoves);
uint64_t getPawnCaptureMask(uint64_t bitboard, int colour);
// uint64_t getPinnedPieces(int colour);
extern uint64_t rookMasks[64];
extern uint64_t bishopMasks[64];
#endif