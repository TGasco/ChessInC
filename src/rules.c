#include <stdio.h>
#include <stdlib.h>
#include "../include/move_vectors.h"
#include "../include/board.h"
#include "../include/magics.h"
// A comprehensive set of legal moves for each piece in Chess

// The following functions are used to determine the legal moves for each piece in Chess.
// The functions are called by the movePiece() function in board.c.

// Declare functions
void computeColourBitboard(int colour);
// int is_valid_position(int row, int col, int colour, Position kingPos);
Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE], 
int enPessant, int kingMoved, int rookMoved[2], Position kingPos, int findingCheck);
MoveVectorList getMoveVectors(Piece piece);
char colToFile(int col);
int findCheck(Piece board[BOARD_SIZE][BOARD_SIZE], int colour, Position kingPos);
void computePieceMove(int pieceType, int colour);
int getBitboardIndex(int pieceType, int colour);
// void prettyPrintBitboard(uint64_t bitboard);
uint64_t* getLookupTable(int pieceType, int colour);
uint64_t generateRookAttacks(int square, uint64_t ownOccupancy, uint64_t opponentOccupancy);
uint64_t generateBishopAttacks(int square, uint64_t ownOccupancy, uint64_t opponentOccupancy);

// int is_valid_position(int row, int col, int colour, Position kingPos) {
//     // A valid position is considered one which is not occupied by a piece of the same colour, 
//     // and is within the bounds of the board

//     // Check that the position is within the bounds of the board
//     if (row < 0 || row > 7 || col < 0 || col > 7) {
//         return 0;
//     }
//     // Check that the position is not occupied by a piece of the same colour
//     // uint64_t bitboard = computeColourBitboard(colour);
//     if (bitboards[colour ? 13 : 14] & (1ULL << (row * 8 + col))) {
//         return 0;
//     }
//     // if (board[row][col].color == colour && board[row][col].type != EMPTY) {
//     //     return 0;
//     // }
//     return 1;
// }

// int findCheck(Piece board[BOARD_SIZE][BOARD_SIZE], int colour, Position kingPos) {
//     // Find if the given colour is in check
//     // If the given colour is in check, then return 1
//     // Otherwise, return 0
//     int check;

//     // Iterate through the board
//     for (int row = 0; row < BOARD_SIZE; ++row) {
//         for (int col = 0; col < BOARD_SIZE; ++col) {
//             // If the current square is occupied by a piece of the opposite colour, then compute the valid moves for that piece
//             // uint64_t bitboard = computeColourBitboard(colour);
//             if (bitboards[colour ? 13 : 14] & (1ULL << (row * 8 + col))) {
//                 Position pos = {row, col};
//                 Position* validMoves = computeValidMoves(pos, board[row][col], board, -1, -1, (int[2]){-1, -1}, kingPos, 1);
//                 // Iterate through the valid moves
//                 for (int i = 0; i < 28; ++i) {
//                     // If the valid move is the same as the position of the king, then the king is in check
//                     if (validMoves[i].row == kingPos.row && validMoves[i].col == kingPos.col) {
//                         // Add the move to the array of check moves
//                         free(validMoves);
//                         return 1;
//                     }
//                 }
//                 free(validMoves);
//             }
//             // if (board[row][col].color != colour && board[row][col].type != EMPTY) {
//             // }
//         }
//     }
//     return 0;
// }

// Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE], 
// int enPessant, int kingMoved, int rookMoved[2], Position kingPos, int findingCheck) {
//     // Compute the valid moves for a piece at the given position on the given board
//     // The valid moves are stored in the given array of valid moves
//     // The number of valid moves is stored in the given integer pointer

//     // Initialize the number of valid moves to 0
//     int numValidMoves = 0;
//     Position* validMoves = malloc(sizeof(Position) * 28);
//     // Get the Move Vectors for the given piece
//     MoveVectorList vectors = getMoveVectors(piece);
//     int numVectors = vectors.count;
//     // If Bishop, Rook or Queen, then the piece can move any number of squares in the given direction
//     int iter = (piece.type == BISHOP || piece.type == ROOK || piece.type == QUEEN) ? 7 : 1;

//     int direction = (piece.color == WHITE) ? 1 : -1;

//     // Handle edge cases for Pawn moves
//     if (piece.type == PAWN) {
//         int newRow = pos.row + direction;
//         int newCol = pos.col;
//         int empty = bitboards[0] & (1ULL << (newRow * 8 + newCol));

//         if (is_valid_position(newRow, newCol, piece.color, kingPos) && empty) {
//             validMoves[numValidMoves].row = newRow;
//             validMoves[numValidMoves].col = newCol;
//             (numValidMoves)++;
            
//             if ((pos.row == 1 && piece.color == WHITE) || (pos.row == 6 && piece.color == BLACK)) {
//                 newRow = pos.row + (2 * direction);
//                 if (is_valid_position(newRow, newCol, piece.color, kingPos) && empty) {
//                     validMoves[numValidMoves].row = newRow;
//                     validMoves[numValidMoves].col = newCol;
//                     (numValidMoves)++;
//                 }
//             }
//         }

//         // Check diagonal captures
//         for (int offset = -1; offset <= 1; offset += 2) {
//             newCol = pos.col + offset;
//             newRow = pos.row + direction;
//             int empty = bitboards[0] & (1ULL << (newRow * 8 + newCol));
//             if (is_valid_position(newRow, newCol, piece.color, kingPos) && !empty) {
//                 validMoves[numValidMoves].row = newRow;
//                 validMoves[numValidMoves].col = newCol;
//                 numValidMoves++;
//             }
//         }

//         // Check for en passant
//         if ((enPessant == pos.col -1 || enPessant == pos.col + 1) && (pos.row == 4 || pos.row == 3)) {
//             newRow = pos.row + direction;
//             newCol = enPessant;
//             int empty = bitboards[0] & (1ULL << (newRow * 8 + newCol));
//             if (is_valid_position(newRow, newCol, piece.color, kingPos) && empty) {
//                 validMoves[numValidMoves].row = newRow;
//                 validMoves[numValidMoves].col = newCol;
//                 numValidMoves++;
//             }
//         }
//     } else {
//         // Iterate through the move vectors
//         for (int i = 0; i < numVectors; ++i) {
//             // Get the current move vector
//             MoveVector vector = vectors.vectors[i];
//             // Compute the new position
//             for (int j = 0; j < iter; ++j) {
//                 int newRow = pos.row + (vector.dy * direction * (j + 1));
//                 int newCol = pos.col + (vector.dx * direction * (j + 1));
//                 int empty = bitboards[0] & (1ULL << (newRow * 8 + newCol));
//                 // Check if the new position is valid
//                 if (is_valid_position(newRow, newCol, piece.color, kingPos)) {
//                     if (piece.type == PAWN && !empty) {
//                         break;
//                     }
//                     // Add the new position to the array of valid moves
//                     Position newPos = {newRow, newCol};
//                     validMoves[numValidMoves] = newPos;
//                     // Increment the number of valid moves
//                     numValidMoves++;
//                     // If piece is of a different colour, then break out of the loop
//                     // uint64_t bitboard = computeColourBitboard(!piece.color);
//                     if (bitboards[!piece.color ? 13 : 14] & (1ULL << (newRow * 8 + newCol))) {
//                         break;
//                     }
//                     // if (!empty && board[newRow][newCol].color != piece.color) {
//                     //     break;
//                     // }

//                 } else {
//                     // Break out of the loop if the new position is not valid
//                     break;
//                 }
//             }
//         }

//         // Check for castling
//         if (piece.type == KING) {
//             // Check for castling on the king side
//             if (kingMoved == 0 && rookMoved[1] == 0) {
//                 int valid = 1;
//                 for (int i = 1; i <= 2; ++i) {
//                     // Check if the squares between the king and the rook are empty in bitboard
//                     if (bitboards[0] & (1ULL << (pos.row * 8 + pos.col + i))) {
//                         valid = 0;
//                         break;
//                     }
//                     // if (board[pos.row][pos.col + i].type != EMPTY) {
//                     //     valid = 0;
//                     //     break;
//                     // }
//                 }
//                 if (valid) {
//                     validMoves[numValidMoves].row = pos.row;
//                     validMoves[numValidMoves].col = pos.col + 2;
//                     numValidMoves++;
//                 }
//             }
//             // Check for castling on the queen side
//             if (kingMoved == 0 && rookMoved[0] == 0) {
//                 int valid = 1;
//                 for (int i = 1; i <= 3; ++i) {
//                     // if (board[pos.row][pos.col - i].type != EMPTY) {
//                     //     valid = 0;
//                     //     break;
//                     // }
//                     // Check if the squares between the king and the rook are empty in bitboard
//                     if (bitboards[0] & (1ULL << (pos.row * 8 + pos.col - i))) {
//                         valid = 0;
//                         break;
//                     }
//                 }
//                 if (valid) {
//                     validMoves[numValidMoves].row = pos.row;
//                     validMoves[numValidMoves].col = pos.col - 2;
//                     numValidMoves++;
//                 }
//             }
//         }
//     }

//     // Fill the rest of the array with empty positions
//     for (int i = numValidMoves; i < 28; ++i) {
//         validMoves[i].row = -1;
//         validMoves[i].col = -1;
//     }

//     if (findingCheck) {
//         return validMoves;
//     } else {
//         // Extrapolate the next move to see if it puts the king in check
//         for (int i = 0; i < numValidMoves; ++i) {
//             // Copy the board
//             Piece newBoard[BOARD_SIZE][BOARD_SIZE];
//             for (int row = 0; row < BOARD_SIZE; ++row) {
//                 for (int col = 0; col < BOARD_SIZE; ++col) {
//                     newBoard[row][col] = board[row][col];
//                 }
//             }
//             // Move the piece
//             newBoard[validMoves[i].row][validMoves[i].col] = newBoard[pos.row][pos.col];
//             newBoard[pos.row][pos.col] = (Piece){EMPTY, WHITE, NULL};


//             // Check if the king is in check
//             // If the current piece is the king, then update the king position
//             // Position tmpKingPos = kingPos;
//             if (piece.type == KING) {
//                 kingPos.row = validMoves[i].row;
//                 kingPos.col = validMoves[i].col;
//             }
//             if (findCheck(newBoard, piece.color, kingPos)) {
//                 // Remove the move from the array of valid moves
//                 for (int j = i; j < numValidMoves; ++j) {
//                     validMoves[j] = validMoves[j + 1];
//                 }
//                 numValidMoves--;
//                 i--;
//             }
//         }
//         return validMoves;
//     }
// }

// !!!!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!!!
// void computeAllValidMoves(Piece board[BOARD_SIZE][BOARD_SIZE], Position** validMovesLookup,
//                             int colour, int enPessant, int kingMoved, 
//                             int rookMoved[2], Position kingPos) {
//     // Compute all the valid moves for the given board
//     // The valid moves are stored in the given array of valid moves
//     // The number of valid moves is stored in the given integer pointer

//     // Initialize the number of valid moves to 0
//     for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
//         validMovesLookup[i] = malloc(sizeof(Position) * 28);
//     }
//     // Iterate through the board
//     for (int row = 0; row < BOARD_SIZE; ++row) {
//         for (int col = 0; col < BOARD_SIZE; ++col) {
//             // If the current square is occupied by a piece of the same colour, then compute the valid moves for that piece
//             // if (board[row][col].color != colour) {
//                 Position pos = {row, col};
//                 Position* moves = computeValidMoves(pos, board[row][col], board, enPessant, kingMoved, rookMoved, kingPos, 0);
//                 // Iterate through the valid moves
//                 for (int i = 0; i < 28; ++i) {
//                     // Add the move to the array of valid moves
//                     // Valid moves index is a flattened 2D array
//                     if (board[row][col].color != colour) {
//                         validMovesLookup[row * BOARD_SIZE + col][i] = moves[i];
//                     } else {
//                         Position empty = {-1, -1};
//                         validMovesLookup[row * BOARD_SIZE + col][0] = empty;
//                     }
//                 }
//                 free(moves);
//             // }
//         }
//     }
//     // print valid moves
//     for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
//         printf("%c%d: ", colToFile(i % BOARD_SIZE), 8 - (i / BOARD_SIZE));
//         for (int j = 0; j < 28; j++) {
//             if (validMovesLookup[i][j].row == -1 && validMovesLookup[i][j].col == -1) {
//                 break;
//             }
//             printf("%c%d ", colToFile(validMovesLookup[i][j].col), 8 - validMovesLookup[i][j].row);
//         }
//         printf("\n");
//     }
// }

int isCheckmate(Position** validMovesLookup) {
    int checkmate = 1;
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        if (validMovesLookup[i] != NULL) {
            for (int j = 0; j < 28; ++j) {
                if (validMovesLookup[i][j].row == -1 && validMovesLookup[i][j].col == -1) {
                    break;
                } else {
                    checkmate = 0;
                }
            }
        }
        if (!checkmate) {
            break;
        }
    }
    return checkmate;
}

MoveVectorList getMoveVectors(Piece piece) {
    // Return the move vectors for the given piece
    switch(piece.type) {
        case PAWN:
            return pawnMoveVectors;
        case KNIGHT:
            return knightMoveVectors;
        case BISHOP:
            return bishopMoveVectors;
        case ROOK:
            return rookMoveVectors;
        case QUEEN:
            return queenMoveVectors;
        case KING:
            return kingMoveVectors;
        default:
            return pawnMoveVectors;
    }
}

void computeMovesBitboard(int colour) {
    // Computes valid moves for all pieces, storing the output in a bitboard
    // uint64_t attackBitboards[15];
    // Iterate over the bitboards for each piece type and colour
    for (int i=1; i<7; i++) {
        int pieceType = i;
        computePieceMove(pieceType, colour);
    }

    for (int i = 1; i < 13; ++i) {
        attackBitboards[0] |= attackBitboards[i];
    }

    attackBitboards[0] = attackBitboards[0];
}

void computePieceMove(int pieceType, int colour) {
    int index = getBitboardIndex(pieceType, colour);
    uint64_t pawnTwoStepMask = 0xFF00000000ULL;
    int pawnShift[] = {8, 16, 7, 9};
    int knightShift[] = {15, 17, 6, 10};
    int rookShift[] = {8, 1};
    int bishopShift[] = {7, 9};
    int kingShift[] = {8, 1, 7, 9};
    switch(pieceType) {
        case PAWN:
            // Pawn moves
            // For white pawns, shift up by 8 bits
            // For black pawns, shift down by 8 bits
            switch (colour) {
                case WHITE:
                    // 2-step pawn moves, only valid if the pawn is on the 2nd rank
                    attackBitboards[index] |= bitboards[index] >> 16;
                    // Apply mask to remove invalid 2-step pawn moves
                    attackBitboards[index] &= pawnTwoStepMask;
                    // Diagonal captures can be computed by shifting the bitboard up or down by 7 or 9 bits
                    // attackBitboards[index] |= bitboards[index] >> 7;
                    // attackBitboards[index] |= bitboards[index] >> 9;
                    // // Cull diagonal moves which do not capture a piece
                    // attackBitboards[index] &= bitboards[14];

                    attackBitboards[index] |= bitboards[index] >> 8;
                    break;
                case BLACK:
                    // 2-step pawn moves, only valid if the pawn is on the 7th rank
                    attackBitboards[index] |= bitboards[index] << 16;
                    // Apply mask to remove invalid 2-step pawn moves
                    // attackBitboards[index] &= (pawnTwoStepMask ^ 56);
                    attackBitboards[index] &= pawnTwoStepMask;
                    // Diagonal captures can be computed by shifting the bitboard up or down by 7 or 9 bits
                    // attackBitboards[index] |= bitboards[index] << 7;
                    // attackBitboards[index] |= bitboards[index] << 9;
                    // // Cull diagonal moves which do not capture a piece
                    // attackBitboards[index] &= bitboards[13];

                    attackBitboards[index] |= bitboards[index] << 8;
                    break;
            }

            break;
        case KNIGHT:
            // Knight moves
            // Knight moves can be computed by shifting the bitboard up or down by 15, 17, 6 or 10 bits
            for (int j = 0; j < 4; ++j) {
                attackBitboards[index] |= bitboards[index] << knightShift[j];
                attackBitboards[index] |= bitboards[index] >> knightShift[j];
            }
            break;
        case BISHOP:
            // Bishop moves
            // Bishop moves can be computed by shifting the bitboard up or down by 7 or 9 bits
            // NOT IMPLEMENTED YET
            break;
        case ROOK:
            // Rook moves
            // Rook moves can be computed by shifting the bitboard up or down by 8 or 1 bits
            // NOT IMPLEMENTED YET
            break;
        case QUEEN:
            // Queen moves
            // Queen moves are the union of Bishop and Rook moves (i.e. the bitwise OR of the Bishop and Rook bitboards)
            // NOT IMPLEMENTED YET
            break;
        case KING:
            // King moves
            // King moves can be computed by shifting the bitboard up or down by 8, 1, 7 or 9 bits
            for (int j = 0; j < 4; ++j) {
                attackBitboards[index] |= bitboards[index] << kingShift[j];
                attackBitboards[index] |= bitboards[index] >> kingShift[j];
                // If shift is not divisible by 8, then apply edge masks to remove invalid moves
                if (kingShift[j] % 8 != 0) {
                    attackBitboards[index] &= RIGHT_WRAP_MASK;
                    attackBitboards[index] &= LEFT_WRAP_MASK;
                }
            }
            // Castling can be computed by shifting the bitboard up or down by 2 bits
            // NOT IMPLEMENTED YET

            // Cull moves which would put the king in check (i.e. moves which overlap with the attack bitboard of the opposite colour)
            attackBitboards[index] &= ~(attackBitboards[colour ? 14 : 13]);
            break;
    }
}

// void computeAllMoves(int colour, uint64_t validMoves[BOARD_SIZE * BOARD_SIZE]) {
//     // Computes valid moves for all pieces, storing the output their respective bitboards
//     // Since all theoretically possible moves are already precomputed, 
//     // this function needs to cull invalid moves based on the current board state
//     uint64_t *lookupTable;
//     for (int i=1; i<7; i++) {
//         int pieceType = i;
//         lookupTable = getLookupTable(pieceType, colour);
//         uint64_t currentPieces = bitboards[pieceType + (colour * 6)];
//         while (currentPieces) {
//             int j = __builtin_ctzll(currentPieces); // get the index of the least significant bit
//             currentPieces &= currentPieces - 1; // clear the least significant bit
//             uint64_t bitboard = lookupTable[j];
//             uint64_t dynamicBitboard = 1ULL << j;
//             uint64_t maskedBitboardL = dynamicBitboard & LEFT_WRAP_MASK;
//             uint64_t maskedBitboardR = dynamicBitboard & RIGHT_WRAP_MASK;



//             // Check if the current square is occupied
//             if (!(bitboards[pieceType + (colour * 6)] & (1ULL << j))) {
//                 continue;
//             } else {
//                 switch(pieceType) {
//                     case PAWN:
//                         // Cull forward moves which are blocked by a piece (any colour)
//                         bitboard &= ~(bitboards[0]);
//                         // Cull diagonal moves which do not capture a piece
//                         switch(colour) {
//                             case WHITE:
//                                 bitboard |= ((maskedBitboardR >> 7) & bitboards[14]);
//                                 bitboard |= ((maskedBitboardL >> 9) & bitboards[14]);
//                                 break;
//                             case BLACK:
//                                 bitboard |= ((maskedBitboardL << 7) & bitboards[13]);
//                                 bitboard |= ((maskedBitboardR << 9) & bitboards[13]);
//                                 break;
//                         }
//                         break;
//                     case KNIGHT:
//                         // No additional culling required for knight moves
//                         break;
//                     case BISHOP:
//                         bitboard = bitboard[]
//                         // bitboard = generateBishopAttacks(j, bitboards[colour ? 14 : 13], bitboards[colour ? 13 : 14]);
//                         break;
//                     case ROOK:
//                         // bitboard = generateRookAttacks(j, bitboards[colour ? 14 : 13], bitboards[colour ? 13 : 14]);
//                         break;
//                     case QUEEN:
//                         // Combine the rook and bishop attacks
//                         // bitboard = generateRookAttacks(j, bitboards[colour ? 14 : 13], bitboards[colour ? 13 : 14]) | generateBishopAttacks(j, bitboards[colour ? 14 : 13], bitboards[colour ? 13 : 14]);
//                         break;
//                     case KING:
//                         // Cull moves which would put the king in check (i.e. moves which overlap with the attack bitboard of the opposite colour)
//                         bitboard &= ~(attackBitboards[colour ? 13 : 14]);
//                         break;
//                 }

                // // For all pieces, cull moves which are blocked by a piece of the same colour
                // bitboard &= ~(bitboards[colour ? 14 : 13]);
                // Add the valid moves to the array of valid moves
                // validMoves[j] = bitboard;

//                 // Add to the global attack bitboard
//                 attackBitboards[pieceType + (colour * 6)] |= bitboard;
//             }
//         }
//     }
// }

void computeAllMoves(int colour, uint64_t validMoves[BOARD_SIZE * BOARD_SIZE]) {
    // Loop over all pieces of the given colour
    for (int pieceType = 1; pieceType < 7; pieceType++)
    {
        uint64_t currentPieces = bitboards[pieceType + (colour * 6)];
        while (currentPieces) {
            // Get square index of the least significant bit
            int j = __builtin_ctzll(currentPieces);
            // Clear the least significant bit
            currentPieces &= currentPieces - 1;
            // Compute the valid moves for the current piece
            validMoves[j] = getValidMoves(pieceType, colour, j, bitboards[0]);
            // Add the attacks to the global attack bitboard
            attackBitboards[pieceType + (colour * 6)] |= validMoves[j];
        }
    }
    
}

uint64_t coordToBitboard(int row, int col) {
    // Convert the given coordinates to a bitboard
    // The bitboard is a 64-bit integer, where each bit represents a square on the board
    return 1ULL << (row * 8 + col);
}

Position* bitboardToPosition(uint64_t bitboard) {
    // Convert the given bitboard to a list of positions
    Position* positions = malloc(sizeof(Position) * (BOARD_SIZE * BOARD_SIZE));
    int numPositions = 0;
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        // Loop through the bitboard
        // If the bit at the current position is 1, then add the position to the list
        if (bitboard & (1ULL << i)) {
            Position pos = {i / BOARD_SIZE, i % BOARD_SIZE};
            positions[numPositions] = pos;
            numPositions++;
        }
    }
    positions[numPositions].row = -1;
    positions[numPositions].col = -1;
    return positions;
}

// void initPawnLookup() {
//     uint64_t pawnTwoStepMaskW = 0xFF00000000ULL;
//     uint64_t pawnTwoStepMaskB = 0xFF000000ULL;
//     // Iterate through the board
//     for (int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) {
//         // Initialize the bitboard to 0
//         pawnAttackLookup[0][i] = 0ULL;
//         pawnAttackLookup[1][i] = 0ULL;

//         uint64_t bitboard = 1ULL << i;

//         // Compute the pawn moves

//         // 2-step pawn moves, only valid if the pawn is on the 2nd rank
//         pawnAttackLookup[0][i] |= bitboard >> 16;
//         pawnAttackLookup[1][i] |= bitboard << 16;
//         // Apply mask to remove invalid 2-step pawn moves
//         pawnAttackLookup[0][i] &= pawnTwoStepMaskW;
//         pawnAttackLookup[1][i] &= pawnTwoStepMaskB;

//         // For white pawns, shift up by 8 bits
//         // For black pawns, shift down by 8 bits
//         pawnAttackLookup[0][i] |= bitboard >> 8;
//         pawnAttackLookup[1][i] |= bitboard << 8;
//     }
// }

// void initKnightLookup() {
//     int knightShift[] = {15, 17, 6, 10};
//     uint64_t knightEdgeMaskL = 0xFCFCFCFCFCFCFCFCULL;
//     uint64_t knightEdgeMaskR = 0x3F3F3F3F3F3F3F3FULL;
//     uint64_t leftWrap = LEFT_WRAP_MASK;
//     uint64_t rightWrap = RIGHT_WRAP_MASK;
//     // Iterate through the board
//     for (int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) {
//         // Initialize the bitboard to 0
//         knightAttackLookup[i] = 0ULL;
//         uint64_t bitboard = 1ULL << i;

//         // Compute the knight moves
//         // The upright 'L' moves can be computed by shifting the bitboard up or down by 15 or 17 bits

//         // Check if the knight is on the left edge of the board, ignore left 'L' moves
//         uint64_t maskedBitboardL1 = bitboard & LEFT_WRAP_MASK;
//         uint64_t maskedBitboardR1 = bitboard & RIGHT_WRAP_MASK;
//         uint64_t maskedBitboardL2 = bitboard & knightEdgeMaskL;
//         uint64_t maskedBitboardR2 = bitboard & knightEdgeMaskR;

//         // The downright 'L' moves can be computed by shifting the bitboard up or down by 6 or 10 bits
//         knightAttackLookup[i] |= maskedBitboardR2 >> 6;
//         knightAttackLookup[i] |= maskedBitboardL2 << 6;

//         knightAttackLookup[i] |= maskedBitboardL2 >> 10;
//         knightAttackLookup[i] |= maskedBitboardR2 << 10;

//         knightAttackLookup[i] |= maskedBitboardL1 << 15;
//         knightAttackLookup[i] |= maskedBitboardR1 >> 15;

//         knightAttackLookup[i] |= maskedBitboardR1 << 17;
//         knightAttackLookup[i] |= maskedBitboardL1 >> 17;
//     }
// }

// // void initRookLookup() {
// //     // Use magic bitboards to compute the rook moves
// //     // NOT IMPLEMENTED YET
// // }

// // void initBishopLookup() {
// //     // Use magic bitboards to compute the bishop moves
// //     // NOT IMPLEMENTED YET
// // }

// // void initQueenLookup() {
// //     // The queen moves are the union of the rook and bishop moves
// //     // NOT IMPLEMENTED YET
// // }

// void initKingLookup() {
//     // Iterate through the board
//     for (int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) {
//         // Initialize the bitboard to 0
//         kingAttackLookup[i] = 0ULL;
//         uint64_t bitboard = 1ULL << i;
//         uint64_t maskedBitboardL = bitboard & LEFT_WRAP_MASK;
//         uint64_t maskedBitboardR = bitboard & RIGHT_WRAP_MASK;

//         // Compute the king moves
//         kingAttackLookup[i] |= maskedBitboardL >> 1;
//         kingAttackLookup[i] |= maskedBitboardR << 1;

//         kingAttackLookup[i] |= maskedBitboardR >> 7;
//         kingAttackLookup[i] |= maskedBitboardL << 7;

//         kingAttackLookup[i] |= bitboard >> 8;
//         kingAttackLookup[i] |= bitboard << 8;

//         kingAttackLookup[i] |= maskedBitboardL >> 9;
//         kingAttackLookup[i] |= maskedBitboardR << 9;
//     }
// }

// void initAllLookups() {
//     initPawnLookup(); // Initialize the pawn lookup table
//     initKnightLookup(); // Initialize the knight lookup table
//     initKingLookup(); // Initialize the king lookup table
// }

// uint64_t generateRookAttacks(int square, uint64_t ownOccupancy, uint64_t opponentOccupancy) {
//     uint64_t rookAttacks = 0ULL;
//     int rookShift[] = {8, 1};

//     // Up
//     for (int i = 1; i < 8; i++) {
//         int nextSquare = square + rookShift[0] * i;
//         if (nextSquare >= 0 && nextSquare < 64) {
//             uint64_t nextSquareBit = (1ULL << nextSquare);
//             if (!(ownOccupancy & nextSquareBit)) {
//                 rookAttacks |= nextSquareBit;
//                 if (opponentOccupancy & nextSquareBit) {
//                     break; // Capture opponent's piece
//                 }
//             } else {
//                 break; // Stop on own piece
//             }
//         } else {
//             break; // Boundary reached
//         }
//     }

//     // Down
//     for (int i = 1; i < 8; i++) {
//         int nextSquare = square - rookShift[0] * i;
//         if (nextSquare >= 0 && nextSquare < 64) {
//             uint64_t nextSquareBit = (1ULL << nextSquare);
//             if (!(ownOccupancy & nextSquareBit)) {
//                 rookAttacks |= nextSquareBit;
//                 if (opponentOccupancy & nextSquareBit) {
//                     break; // Capture opponent's piece
//                 }
//             } else {
//                 break; // Stop on own piece
//             }
//         } else {
//             break; // Boundary reached
//         }
//     }

//     // Right
//     for (int i = 1; i < 8; i++) {
//         int nextSquare = square + rookShift[1] * i;
//         if (nextSquare >= 0 && nextSquare < 64 && nextSquare % 8 != 0) {
//             uint64_t nextSquareBit = (1ULL << nextSquare);
//             if (!(ownOccupancy & nextSquareBit)) {
//                 rookAttacks |= nextSquareBit;
//                 if (opponentOccupancy & nextSquareBit) {
//                     break; // Capture opponent's piece
//                 }
//             } else {
//                 break; // Stop on own piece
//             }
//         } else {
//             break; // Boundary reached
//         }
//     }

//     // Left
//     for (int i = 1; i < 8; i++) {
//         int nextSquare = square - rookShift[1] * i;
//         if (nextSquare >= 0 && nextSquare < 64 && (nextSquare + 1) % 8 != 0) {
//             uint64_t nextSquareBit = (1ULL << nextSquare);
//             if (!(ownOccupancy & nextSquareBit)) {
//                 rookAttacks |= nextSquareBit;
//                 if (opponentOccupancy & nextSquareBit) {
//                     break; // Capture opponent's piece
//                 }
//             } else {
//                 break; // Stop on own piece
//             }
//         } else {
//             break; // Boundary reached
//         }
//     }

//     return rookAttacks;
// }

// uint64_t generateBishopAttacks(int square, uint64_t ownOccupancy, uint64_t opponentOccupancy) {
//     uint64_t bishopAttacks = 0ULL;
//     int bishopShift[] = {7, 9, -7, -9};

//     // Diagonal movements (4 directions)
//     for (int direction = 0; direction < 4; ++direction) {
//         for (int i = 1; i < 8; ++i) {
//             int nextSquare = square + bishopShift[direction] * i;
//             if (nextSquare >= 0 && nextSquare < 64) {
//                 // Check if the move reaches the board's edge
//                 if ((nextSquare % 8 == 0 && (direction == 1 || direction == 2)) || 
//                     ((nextSquare + 1) % 8 == 0 && (direction == 0 || direction == 3))) {
//                     break;
//                 }
//                 uint64_t nextSquareBit = (1ULL << nextSquare);
//                 if (!(ownOccupancy & nextSquareBit)) {
//                     bishopAttacks |= nextSquareBit;
//                     if (opponentOccupancy & nextSquareBit) {
//                         break; // Capture opponent's piece
//                     }
//                 } else {
//                     break; // Stop on own piece
//                 }
//             } else {
//                 break; // Boundary reached
//             }
//         }
//     }

//     return bishopAttacks;
// }



int isCapture(uint64_t *bitboard, int colour) {
    // Check if the move is a capture
    // If the move is a capture, then return 1
    // Otherwise, return 0
    // uint64_t bitboard = computeColourBitboard(colour);
    if (bitboards[colour ? 13 : 14] & *bitboard) {
        return 1;
    }
    return 0;
}

void cullInvalidMoves(uint64_t* bitboard, int colour) {
    // Cull moves which are invalid, because:
    // 1. The move would put the king in check
    // 2. The move would capture a piece of the same colour
    // 3. The move would not capture a piece of the opposite colour (for pawns only)
    // 4. Castling would move the king through check or into check

    // Check if the move would put the king in check

    // Check if the move would capture a piece of the same colour
    *bitboard &= ~(bitboards[colour ? 13 : 14]);
}


// void prettyPrintBitboard(uint64_t bitboard) {
//     // Print the given bitboard to the console
//     // The bitboard is a 64-bit integer, where each bit represents a square on the board
//     // The least significant bit represents the square a1, and the most significant bit represents the square h8

//     // Iterate through the bitboard
//     for (int row = 0; row < BOARD_SIZE; ++row) {
//         for (int col = 0; col < BOARD_SIZE; ++col) {
//             // Print the bit at the current position
//             printf("%llu ", bitboard & 1);
//             // Shift the bitboard to the right by 1
//             bitboard >>= 1;
//         }
//         printf("\n");
//     }
// }

void computeGlobalBitboard() {
    bitboards[0] = bitboards[13] | bitboards[14];
}

void computeColourBitboard(int colour) {
    switch (colour) {
        case WHITE:
            bitboards[13] = bitboards[1] | bitboards[2] | bitboards[3] | bitboards[4] | bitboards[5] | bitboards[6];
            break;
        case BLACK:
            bitboards[14] = bitboards[7] | bitboards[8] | bitboards[9] | bitboards[10] | bitboards[11] | bitboards[12];
            break;
    }
}

void updateBitboards() {
    computeColourBitboard(WHITE);
    computeColourBitboard(BLACK);
    computeGlobalBitboard();
}

int getBitboardIndex(int pieceType, int colour) {
    return pieceType + (colour * 6);
}

uint64_t* getLookupTable(int pieceType, int colour) {
    switch(pieceType) {
        case PAWN:
            switch(colour) {
                case WHITE:
                    return pawnAttackLookup[0];
                case BLACK:
                    return pawnAttackLookup[1];
            }
        case KNIGHT:
            return knightAttackLookup;
        case BISHOP:
            return bishopAttackLookup;
        case ROOK:
            return rookAttackLookup;
        case QUEEN:
            return rookAttackLookup;
        case KING:
            return kingAttackLookup;
        default:
            return pawnAttackLookup[0];
    }
}

void parseFENToBitboard(char* fen) {
    // Parse the FEN string to a bitboard
    // The FEN string is stored in the global variable fen
    // The bitboard is stored in the global variable bitboard
    // The bitboard is a 64-bit integer, where each bit represents a square on the board
    // The least significant bit represents the square a1, and the most significant bit represents the square h8
    // The bitboard is initialized to 0

    int squareIndex = 56; // Start at a8 (the 56th index for a 64-bit integer)

    // Iterate through the FEN string
    while (*fen != '\0') {
        // If the current character is a digit, then shift the bitboard to the left by the given number of bits
        if (*fen >= '1' && *fen <= '8') {
            int shift = *fen - '0';
            squareIndex += shift;
            fen++;
            continue; // Skip the rest of the loop and continue with the next character
        } else if (*fen == '/') {
            squareIndex -= 16; // Skip to the next rank (8 squares in a rank)
        } else {
            // If the current character is a letter, then set the bit at the current position to 1
            int pieceIndex = -1;

            switch (*fen) {
                case 'p':
                    pieceIndex = PAWN + 6;
                    break;
                case 'P':
                    pieceIndex = PAWN;
                    break;
                case 'n':
                    pieceIndex = KNIGHT + 6;
                    break;
                case 'N':
                    pieceIndex = KNIGHT;
                    break;
                case 'b':
                    pieceIndex = BISHOP + 6;
                    break;
                case 'B':
                    pieceIndex = BISHOP;
                    break;
                case 'r':
                    pieceIndex = ROOK + 6;
                    break;
                case 'R':
                    pieceIndex = ROOK;
                    break;
                case 'q':
                    pieceIndex = QUEEN + 6;
                    break;
                case 'Q':
                    pieceIndex = QUEEN;
                    break;
                case 'k':
                    pieceIndex = KING + 6;
                    break;
                case 'K':
                    pieceIndex = KING;
                    break;
            }
        
            // Set the bit at the current position to 1 for the respective bitboard
            if (pieceIndex != -1) {
                uint64_t bitPosition = 1ULL << squareIndex;
                bitboards[pieceIndex] |= bitPosition;
            }
            squareIndex++;
        }
        // Increment the FEN string pointer
        fen++;
    }

    // Flip the vertical orientation of the bitboard
    // This is necessary because the FEN string starts at the 8th rank, whereas the bitboard starts at the 1st rank
    for (int i=1; i<13; i++) {
        verticalFlip(&bitboards[i]);
    }
    // Global bitboard is the bitwise OR of all the bitboards
    computeColourBitboard(WHITE);
    computeColourBitboard(BLACK);
    computeGlobalBitboard();
}