#include "../include/board.h"

#ifndef HELPERMETHODS_H
#define HELPERMETHODS_H

char pieceToChar(Piece piece);

char colToFile(int col);

void moveToNotation(Piece piece, Position posFrom, Position posTo, int capture);

int squareToBitIndex(const char* square);

#endif // HELPERMETHODS_H