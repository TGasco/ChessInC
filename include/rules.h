#ifndef RULES_H
#define RULES_H

int is_valid_position(int row, int col, int colour, Position kingPos, Piece board[BOARD_SIZE][BOARD_SIZE]);
Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE],
 int enPessant, int kingMoved, int rookMoved[2], Position kingPos, int findingCheck);
char colToFile(int col);
int findCheck(Piece board[BOARD_SIZE][BOARD_SIZE], int colour, Position kingPos);
void computeAllValidMoves(Piece board[BOARD_SIZE][BOARD_SIZE], Position** validMovesLookup, int colour,
 int enPessant, int kingMoved, int rookMoved[2], Position kingPos);
int isCheckmate(Position** validMovesLookup);

#endif