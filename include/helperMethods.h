#include "../include/board.h"

#ifndef HELPERMETHODS_H
#define HELPERMETHODS_H

char pieceToChar(Piece piece);

char colToFile(int col);

void moveToNotation(Piece piece, Position posFrom, Position posTo, int capture);

int squareToBitIndex(const char* square);

void printValidMoves(Move* validMoves, int numValidMoves);

int posToSquare(Position pos);

Position squareToPos(int square);

uint64_t indexToBitboard(int square);

#endif // HELPERMETHODS_H