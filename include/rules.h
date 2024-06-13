#ifndef RULES_H
#define RULES_H

uint64_t parseFENToBitboard(char* fen);
uint64_t computeColourBitboard(int colour);
void updateBitboards();
uint64_t coordToBitboard(int row, int col);
int getBitboardIndex(int pieceType, int colour);
Position* bitboardToPosition(uint64_t bitboard);
void computeAllMoves(uint64_t* validMoves);
// void getPseudoValidMoves(int colour, uint64_t* validMoves, int recursive);
int isCapture(uint64_t bitboard, int colour, int pieceType);
void removePiece(uint64_t *bitboard, uint64_t bit);
int isCheck(int colour);
int isCheckmate(int colour);
int isEndGame();
void promotePiece(uint64_t bit, int pieceType, int colour);
// int makeMove(Piece selectedPiece, int squareFrom, int squareTo, uint64_t* validMoves);
// int makeMove(Piece selectedPiece, int squareFrom, int squareTo, uint64_t* validMoves, int simulate);
int makeMove(Move move, int simulate);
void validateMoves(int colour, uint64_t* validMoves);
int countValidMoves(uint64_t* validMoves);
int isMoveValid(Move move, uint64_t* validMoves);
#endif