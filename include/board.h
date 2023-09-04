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

// Asset path for the piece sprites
static const char* pieceSprites[] = {
    "../assets/pieces/white_pawn.png",
    "../assets/pieces/white_knight.png",
    "../assets/pieces/white_bishop.png",
    "../assets/pieces/white_rook.png",
    "../assets/pieces/white_queen.png",
    "../assets/pieces/white_king.png",
    "../assets/pieces/black_pawn.png",
    "../assets/pieces/black_knight.png",
    "../assets/pieces/black_bishop.png",
    "../assets/pieces/black_rook.png",
    "../assets/pieces/black_queen.png",
    "../assets/pieces/black_king.png"
};

typedef struct {
    PieceType type;
    PieceColor color;
    const char* sprite;
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
