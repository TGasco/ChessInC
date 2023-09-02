#ifndef RULES_H
#define RULES_H

int is_valid_position(int row, int col, int colour, Piece board[BOARD_SIZE][BOARD_SIZE]);
Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE]);

#endif