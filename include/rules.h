#ifndef RULES_H
#define RULES_H

// int is_valid_position(int row, int col, int colour, Position kingPos);
Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE],
 int enPessant, int kingMoved, int rookMoved[2], Position kingPos, int findingCheck);
int findCheck(Piece board[BOARD_SIZE][BOARD_SIZE], int colour, Position kingPos);
// void computeAllValidMoves(Piece board[BOARD_SIZE][BOARD_SIZE], Position** validMovesLookup, int colour,
//  int enPessant, int kingMoved, int rookMoved[2], Position kingPos);
int isCheckmate(Position** validMovesLookup);
uint64_t parseFENToBitboard(char* fen);
// void prettyPrintBitboard(uint64_t bitboard);
uint64_t computeColourBitboard(int colour);
void updateBitboards();
void computeMovesBitboard(int colour);
uint64_t coordToBitboard(int row, int col);
// void initAllLookups();
int getBitboardIndex(int pieceType, int colour);
Position* bitboardToPosition(uint64_t bitboard);
uint64_t* getLookupTable(int pieceType, int colour);
void computeAllMoves(int colour, uint64_t validMoves[BOARD_SIZE * BOARD_SIZE]);
#endif