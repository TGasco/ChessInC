#include <stdio.h>
#include <string.h>
#include "../include/board.h"

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

char colToFile(int col) {
    // Convert the given column number to a file letter
    switch(col) {
        case 0:
            return 'a';
        case 1:
            return 'b';
        case 2:
            return 'c';
        case 3:
            return 'd';
        case 4:
            return 'e';
        case 5:
            return 'f';
        case 6:
            return 'g';
        case 7:
            return 'h';
    }
    return '?';
}

void moveToNotation(Piece piece, Position posFrom, Position posTo, int capture) {
    // Convert a move to algebraic notation
    char notation[10];  // Assuming a reasonable maximum length for the notation

    if (piece.type != PAWN) {
        snprintf(notation, sizeof(notation), "%c", pieceToChar(piece));
    }

    if (capture) {
        if (piece.type == PAWN) {
            snprintf(notation, sizeof(notation), "%c", colToFile(posFrom.col));
        }
        strcat(notation, "x");
    }

    printf("%s%c%d\n", notation, colToFile(posTo.col), 8 - posTo.row);
}


int squareToBitIndex(const char* square) {
    // Convert the given square to a bit index
    int file = square[0] - 'a';
    int rank= 8 - (square[1] - '0');
    return rank * 8 + file;
}