// board.h
#include <stdint.h>

#ifndef BOARD_H
#define BOARD_H

#define BOARD_SIZE 8
#define SQUARE_SIZE 50

#define RIGHT_WRAP_MASK 0x7F7F7F7F7F7F7F7FULL;
#define LEFT_WRAP_MASK 0xFEFEFEFEFEFEFEFEULL;
#define UP_WRAP_MASK 0xFFFFFFFFFFFFFF00ULL;
#define DOWN_WRAP_MASK 0x00FFFFFFFFFFFFFFULL;

// Flips the bitboard vertically (used for black pieces)
#define FLIP(sq) ((sq)^56)

typedef enum {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PieceType;

typedef enum {
    WHITE, BLACK
} PieceColor;

typedef enum {
    GLOBAL, WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    WHITE_GLOB, BLACK_GLOB
} BitboardIndex;

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

typedef struct Move {
    Piece piece;
    int from;
    int to;
    int score;
} Move;

typedef struct {
    uint64_t bitboards[15];
    uint64_t attackBitboards[15];
    uint64_t enPassantMask;
    uint8_t castleRights;
} BoardState;



Piece board[BOARD_SIZE][BOARD_SIZE];

// Square encoding
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

// Defines the bitboards for each piece type and color
// 0th index is the global bitboard for all pieces
// 1-6 are the bitboards for white pieces
// 7-12 are the bitboards for black pieces
// 13 is the global bitboard for all white pieces
// 14 is the global bitboard for all black pieces
// Ensure that the global bitboards are updated after each move to avoid desync
// extern uint64_t bitboards[15];
// extern uint64_t attackBitboards[15];
extern uint64_t pawnAttackLookup[2][64];
extern uint64_t knightAttackLookup[64];
extern uint64_t* rookAttackLookup[64];
extern uint64_t* bishopAttackLookup[64];
extern uint64_t* queenAttackLookup[64];
extern uint64_t kingAttackLookup[64];

// extern uint64_t enPassantMask; // Mask tracking the en passant square
extern uint64_t promotionMask[2]; // Mask tracking the promotion squares
extern uint64_t kingSideCastleMask[2]; // Mask tracking the king side castling squares
extern uint64_t queenSideCastleMask[2]; // Mask tracking the queen side castling squares
// extern uint8_t castleRights; // Bitmask tracking the castling rights

extern BoardState currentState;
extern BoardState prevState;

extern int pawnPieceTable[2][64];
extern int knightPieceTable[2][64];
extern int bishopPieceTable[2][64];
extern int rookPieceTable[2][64];
extern int queenPieceTable[2][64];
extern int kingPieceTable[2][64];

#define WHITE_KINGSIDE  0x1
#define WHITE_QUEENSIDE 0x2
#define BLACK_KINGSIDE  0x4
#define BLACK_QUEENSIDE 0x8

PieceType getType(Position pos);
PieceColor getColor(Position pos);
Piece (*initBoard())[BOARD_SIZE][BOARD_SIZE];
void printBoard();
char* getPieceSprite(PieceType type, PieceColor color);
void verticalFlip(uint64_t* bitboard);
void prettyPrintBitboard(uint64_t bitboard);
int getBoardAtIndex(int index, int colour);
Piece getPieceAtSquare(int square);
void saveBoardState(BoardState* state);
void restoreBoardState(BoardState* state);

#endif // BOARD_H
