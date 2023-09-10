#include <stdio.h>
#include "../include/board.h"
#define BOARD_SIZE 8

Piece board[BOARD_SIZE][BOARD_SIZE];

PieceType getType(Position pos) {
    return board[pos.row][pos.col].type;
}

PieceColor getColor(Position pos) {
    return board[pos.row][pos.col].color;
}

Piece (*initBoard())[BOARD_SIZE][BOARD_SIZE] {
    // Initialize the back rank for both white and black pieces
    PieceType backRank[] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    for (int col = 0; col < BOARD_SIZE; col++) {
        board[0][col] = (Piece){backRank[col], WHITE, pieceSprites[(int)(backRank[col]) - 1]};
        board[7][col] = (Piece){backRank[col], BLACK, pieceSprites[((int)(backRank[col])) + 5]};
    }

    // Initialize the front rank for both white and black pawns
    for (int col = 0; col < BOARD_SIZE; col++) {
        board[1][col] = (Piece){PAWN, WHITE, pieceSprites[0]};
        board[6][col] = (Piece){PAWN, BLACK, pieceSprites[6]};
    }

    // Initialize the rest of the board with empty squares
    for (int row = 2; row < 6; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = (Piece){EMPTY, WHITE, NULL};
        }
    }

    return &board;
}

char pieceToChar(Piece piece) {
    switch(piece.color) {
        case WHITE:
            switch (piece.type) {
                case EMPTY:
                    return '-';
                case PAWN:
                    return 'P';
                case KNIGHT:
                    return 'N';
                case BISHOP:
                    return 'B';
                case ROOK:
                    return 'R';
                case QUEEN:
                    return 'Q';
                case KING:
                    return 'K';
                default:
                    return '?';
            }
        case BLACK:
            switch (piece.type) {
                case EMPTY:
                    return '-';
                case PAWN:
                    return 'p';
                case KNIGHT:
                    return 'n';
                case BISHOP:
                    return 'b';
                case ROOK:
                    return 'r';
                case QUEEN:
                    return 'q';
                case KING:
                    return 'k';
                default:
                    return '?';
            }
        default:
            return '?';
    }
}

void printBoard() {
    // Print the board to the console
    printf("\n");
    for (int row = 0; row < BOARD_SIZE; row++) {
        printf("%d ", 8 - row);
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("%c ", pieceToChar(board[row][col]));
        }
        printf("\n");
    }
}

int squareToBitIndex(const char* square) {
    // Convert the given square to a bit index
    int file = square[0] - 'a';
    int rank= 8 - (square[1] - '0');
    return rank * 8 + file;
}
