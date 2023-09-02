// board.h

#ifndef BOARD_H
#define BOARD_H

#define BOARD_SIZE 8

typedef enum {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PieceType;

typedef enum {
    WHITE, BLACK
} PieceColor;

typedef struct {
    PieceType type;
    PieceColor color;
} Piece;

typedef struct {
    int row;
    int col;
} Position;

Piece board[BOARD_SIZE][BOARD_SIZE];

int isValidAndEmpty(Position pos);
PieceType getType(Position pos);
PieceColor getColor(Position pos);
Piece (*initBoard())[BOARD_SIZE][BOARD_SIZE];
char pieceToChar(Piece piece);
void printBoard();

#endif // BOARD_H
