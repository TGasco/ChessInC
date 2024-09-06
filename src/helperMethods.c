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
        default:
            return '?';
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

void printValidMoves(Move* validMoves, int numValidMoves) {
    // Print the valid moves to the console
    printf("Valid moves: ");
    int move = 0;
    int idx = numValidMoves;
    while (move < idx) {
        if (validMoves[move].to == -1) {
            printf("End: Total moves: %d\n", move);
            break;
        }
        printf("%c%d Score: %d, ", colToFile(validMoves[move].to % 8), 8 - validMoves[move].to / 8, validMoves[move].score);
        move++;
    }
    printf("\n");
}


int squareToBitIndex(const char* square) {
    // Convert the given square to a bit index
    int file = square[0] - 'a';
    int rank= 8 - (square[1] - '0');
    return rank * 8 + file;
}

uint64_t indexToBitboard(int square) {
    // Convert the given square to a bitboard
    return (1ULL << square);
}

int posToSquare(Position pos) {
    // Convert the given position to a square index
    return pos.row * BOARD_SIZE + pos.col;
}

Position squareToPos(int square) {
    // Convert the given square index to a position
    return (Position){square / BOARD_SIZE, square % BOARD_SIZE};
}

int countBits(uint64_t x) {
    return __builtin_popcountll(x);
}