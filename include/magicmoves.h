#ifndef MAGICMOVES_H
#define MAGICMOVES_H

#define U64 unsigned long long

// Define bit manipulation macros
#define GETBIT(bitboard, square) ((bitboard) & (1ULL << (square)))
#define SETBIT(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define POPBIT(bitboard, square) (GETBIT(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)

U64 findMagic(int square, int relevantBits, int isBishop);
void initMagics();
void initSliderAttacks(int isBishop);
U64 getBishopAttacks(int square, U64 occupancy);
U64 getRookAttacks(int square, U64 occupancy);
enum { rook, bishop };

#endif