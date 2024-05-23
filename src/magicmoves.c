#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/magicmoves.h"
// #define U64 unsigned long long

// // Define bit manipulation macros
// #define GETBIT(bitboard, square) ((bitboard) & (1ULL << (square)))
// #define SETBIT(bitboard, square) ((bitboard) |= (1ULL << (square)))
// #define POPBIT(bitboard, square) (GETBIT(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)

// // rook & bishop flags
// enum { rook, bishop };

// rook relevant occupancy bits
int rookRelevantBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// bishop relevant occupancy bits
int bishopRelevantBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

U64 rookMagics[64];
U64 bishopMagics[64];

U64 rookMasks[64];
U64 bishopMasks[64];

U64 bishopAttacks[64][512];
U64 rookAttacks[64][4096];

unsigned int seed = 123456789;

// PRNG generator for 32-bit numbers
unsigned int generate_random_number()
{
	// XOR shift algorithm
	unsigned int x = seed;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	seed = x;
	return x;
}

// Generate random 64-bit number
U64 randomU64() {
    // init numbers to randomize
    U64 u1, u2, u3, u4;
    
    // randomize numbers
    u1 = (U64)(generate_random_number()) & 0xFFFF;
    u2 = (U64)(generate_random_number()) & 0xFFFF;
    u3 = (U64)(generate_random_number()) & 0xFFFF;
    u4 = (U64)(generate_random_number()) & 0xFFFF;
    
    // shuffle bits and return
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

U64 randomFewBits() {
    return randomU64() & randomU64() & randomU64();
}

// count bits (Brian Kernighan's way)
int countBits(U64 bitboard) {
  // bit count
  int count = 0;
  
  // pop bits untill bitboard is empty
  while (bitboard)
  {
      // increment count
      count++;
      
      // consecutively reset least significant 1st bit
      bitboard &= bitboard - 1;
  }
  
  // return bit count
  return count;
}

// get index of LS1B in bitboard
int getLS1BIndex(U64 bitboard) {
    // make sure bitboard is not empty
    if (bitboard != 0)
        // convert rankailing zeros before LS1B to ones and count them
        return countBits((bitboard & -bitboard) - 1);
    
    // otherwise
    else
        // return illegal index
        return -1;
}

// set occupancies
U64 setOccupancy(int index, int bits_in_mask, U64 attack_mask)
{
    // occupancy map
    U64 occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = getLS1BIndex(attack_mask);
        
        // pop LS1B in attack map
        POPBIT(attack_mask, square);
        
        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }
    
    // return occupancy map
    return occupancy;
}

// Mask bishop attacks
U64 maskBishopAttacks(int square) {
    // attacks map
    U64 attacks = 0ULL;
    int f, r;
    // get square rank and file
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks
    for (r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    
    // return attacks map
    return attacks;
}

// Mask rook attacks
U64 maskRookAttacks(int square) {
    // attacks map
    U64 attacks = 0ULL;
    int r, f;
    // get square rank and file
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks
    for (r = rank + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + file));
    for (r = rank - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + file));
    for (f = file + 1; f <= 6; f++) attacks |= (1ULL << (rank * 8 + f));
    for (f = file - 1; f >= 1; f--) attacks |= (1ULL << (rank * 8 + f));
    
    // return attacks map
    return attacks;
}

// Generate Bishop Attacks (on the fly)
U64 bishopAttacksOTF(int square, U64 block) {
    // attacks map
    U64 attacks = 0ULL;
    int r, f;
    // get square rank and file
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks
    for (r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    for (r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    for (r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    for (r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    // return attacks map
    return attacks;
}

// Generate Rook Attacks (on the fly)
U64 rookAttacksOTF(int square, U64 block) {
    // attacks map
    U64 attacks = 0ULL;
    int r, f;
    // get square rank and file
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks
    for (r = rank + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + file));
        if (block & (1ULL << (r * 8 + file))) break;
    }
    for (r = rank - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + file));
        if (block & (1ULL << (r * 8 + file))) break;
    }
    for (f = file + 1; f <= 7; f++) {
        attacks |= (1ULL << (rank * 8 + f));
        if (block & (1ULL << (rank * 8 + f))) break;
    }
    for (f = file - 1; f >= 0; f--) {
        attacks |= (1ULL << (rank * 8 + f));
        if (block & (1ULL << (rank * 8 + f))) break;
    }
    
    // return attacks map
    return attacks;
}

// Generating Magic Numbers

U64 findMagic(int square, int relevantBits, int isBishop) {
    U64 occupancies[4096];

    U64 attacks[4096];

    U64 usedAttacks[4096];

    // Mask attack
    U64 mask = isBishop ? maskBishopAttacks(square) : maskRookAttacks(square);

    // Occupancy variations
    int occupancyVariations = 1 << relevantBits;

    // Loop over all occupancy variations
    for (int i = 0; i < occupancyVariations; i++) {
        // Generate occupancy
        occupancies[i] = setOccupancy(i, relevantBits, mask);

        // Generate attacks
        attacks[i] = isBishop ? bishopAttacksOTF(square, occupancies[i]) : rookAttacksOTF(square, occupancies[i]);
    }

    // Validate magic numbers
    for (int i = 0; i < 1000000; i++) {
        // Generate random magic number
        U64 magic = randomFewBits();

        if(countBits((mask * magic) & 0xFF00000000000000ULL) < 6) continue;

        // Reset used attacks
        memset(usedAttacks, 0ULL, sizeof(usedAttacks));

        int count, fail;

        // Loop over all occupancy variations
        for (count = 0, fail = 0; !fail && count < occupancyVariations; count++) {
            // Get index
            int index = (int)((occupancies[count] * magic) >> (64 - relevantBits));

            // Check if index is used
            if (usedAttacks[index] == 0ULL) {
                // Set index
                usedAttacks[index] = attacks[count];
            } else if (usedAttacks[index] != attacks[count]) fail = 1;
        }

        // If magic number is valid
        if (!fail) return magic;
    }

    // Return 0 if no magic number is found
    printf("No magic number found for square %d\n", square);
    return 0ULL;
}

void initMagics() {
    // Loop over all squares
    for (int square = 0; square < 64; square++) {
        // Get relevant bits
        int relevantBits = rookRelevantBits[square];

        // Find rook magic
        rookMagics[square] = findMagic(square, relevantBits, rook);

        // Get relevant bits
        relevantBits = bishopRelevantBits[square];

        // Find bishop magic
        bishopMagics[square] = findMagic(square, relevantBits, bishop);
    }
}

void initSliderAttacks(int isBishop) {
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishopMasks[square] = maskBishopAttacks(square);
        rookMasks[square] = maskRookAttacks(square);
        
        // init current mask
        U64 mask = isBishop ? maskBishopAttacks(square) : maskRookAttacks(square);
        
        // count attack mask bits
        int bit_count = countBits(mask);
        
        // occupancy variations count
        int occupancy_variations = 1 << bit_count;
        
        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
            // bishop
            if (isBishop)
            {
                // init occupancies, magic index & attacks
                U64 occupancy = setOccupancy(count, bit_count, mask);
                U64 magic_index = occupancy * bishopMagics[square] >> 64 - bishopRelevantBits[square];
                bishopAttacks[square][magic_index] = bishopAttacksOTF(square, occupancy);                
            }
            
            // rook
            else
            {
                // init occupancies, magic index & attacks
                U64 occupancy = setOccupancy(count, bit_count, mask);
                U64 magic_index = occupancy * rookMagics[square] >> 64 - rookRelevantBits[square];
                rookAttacks[square][magic_index] = rookAttacksOTF(square, occupancy);                
            }
        }
    }
}

U64 getBishopAttacks(int square, U64 occupancy) {
    // Get occupancy variations
    U64 magicIndex = occupancy * bishopMagics[square] >> (64 - bishopRelevantBits[square]);

    // Return attacks
    return bishopAttacks[square][magicIndex];
}

U64 getRookAttacks(int square, U64 occupancy) {
    // Get occupancy variations
    U64 magicIndex = occupancy * rookMagics[square] >> (64 - rookRelevantBits[square]);

    // Return attacks
    return rookAttacks[square][magicIndex];
}