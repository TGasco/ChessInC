#include <stdio.h>
#include <stdlib.h>
#include "../include/move_vectors.h"
#include "../include/board.h"
// A comprehensive set of legal moves for each piece in Chess

// The following functions are used to determine the legal moves for each piece in Chess.
// The functions are called by the movePiece() function in board.c.

// Declare functions
int is_valid_position(int row, int col, int colour, Piece board[BOARD_SIZE][BOARD_SIZE]);
Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE], int enPessant, int kingMoved, int rookMoved[2]);
MoveVectorList getMoveVectors(Piece piece);
char colToFile(int col);
int findCheck(Piece board[BOARD_SIZE][BOARD_SIZE], int colour, Position kingPos);

int is_valid_position(int row, int col, int colour, Piece board[BOARD_SIZE][BOARD_SIZE]) {
    // A valid position is considered one which is not occupied by a piece of the same colour, 
    // and is within the bounds of the board

    // Check that the position is within the bounds of the board
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        return 0;
    }
    // Check that the position is not occupied by a piece of the same colour
    if (board[row][col].color == colour && board[row][col].type != EMPTY) {
        return 0;
    }
    return 1;
}

int findCheck(Piece board[BOARD_SIZE][BOARD_SIZE], int colour, Position kingPos) {
    // Find if the given colour is in check
    // If the given colour is in check, then return 1
    // Otherwise, return 0

    // Find the position of the king
    int check = 0;
    printf("King position: %d, %d\n", kingPos.row, kingPos.col);

    // Iterate through the board
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            // If the current square is occupied by a piece of the opposite colour, then compute the valid moves for that piece
            if (board[row][col].color != colour && board[row][col].type != EMPTY) {
                Position pos = {row, col};
                Position* validMoves = computeValidMoves(pos, board[row][col], board, -1, -1, (int[2]){-1, -1});
                // Iterate through the valid moves
                for (int i = 0; i < 28; ++i) {
                    // If the valid move is the same as the position of the king, then the king is in check
                    if (validMoves[i].row == kingPos.row && validMoves[i].col == kingPos.col) {
                        printf("Move %c%d%c%d puts the king in check\n", colToFile(col), row + 1, colToFile(kingPos.col), kingPos.row + 1);
                        // return 1;
                        check = 1;
                    }
                }
            }
        }
    }
    return check;
}

Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE], int enPessant, int kingMoved, int rookMoved[2]) {
    // Compute the valid moves for a piece at the given position on the given board
    // The valid moves are stored in the given array of valid moves
    // The number of valid moves is stored in the given integer pointer

    // Initialize the number of valid moves to 0
    int numValidMoves = 0;
    Position* validMoves = malloc(sizeof(Position) * 28);
    for (int i = 0; i < 28; i++) {
        validMoves[i].row = -1;
        validMoves[i].col = -1;
    }
    // Get the Move Vectors for the given piece
    MoveVectorList vectors = getMoveVectors(piece);
    int numVectors = vectors.count;
    // If Bishop, Rook or Queen, then the piece can move any number of squares in the given direction
    int iter = (piece.type == BISHOP || piece.type == ROOK || piece.type == QUEEN) ? 7 : 1;

    int direction = (piece.color == WHITE) ? 1 : -1;
    // Handle edge cases for Pawn moves
    if (piece.type == PAWN) {
        int newRow = pos.row + direction;
        int newCol = pos.col;

        if (is_valid_position(newRow, newCol, piece.color, board) && board[newRow][newCol].type == EMPTY) {
            validMoves[numValidMoves].row = newRow;
            validMoves[numValidMoves].col = newCol;
            (numValidMoves)++;
            
            if ((pos.row == 1 && piece.color == WHITE) || (pos.row == 6 && piece.color == BLACK)) {
                newRow = pos.row + (2 * direction);
                if (is_valid_position(newRow, newCol, piece.color, board) && board[newRow][newCol].type == EMPTY) {
                    validMoves[numValidMoves].row = newRow;
                    validMoves[numValidMoves].col = newCol;
                    (numValidMoves)++;
                }
            }
        }

        // Check diagonal captures
        for (int offset = -1; offset <= 1; offset += 2) {
            newCol = pos.col + offset;
            newRow = pos.row + direction;
            if (is_valid_position(newRow, newCol, piece.color, board) && board[newRow][newCol].type != EMPTY) {
                validMoves[numValidMoves].row = newRow;
                validMoves[numValidMoves].col = newCol;
                numValidMoves++;
            }
        }

        // Check for en passant
        if ((enPessant == pos.col -1 || enPessant == pos.col + 1) && (pos.row == 4 || pos.row == 3)) {
            newRow = pos.row + direction;
            newCol = enPessant;
            if (is_valid_position(newRow, newCol, piece.color, board) && board[newRow][newCol].type == EMPTY) {
                validMoves[numValidMoves].row = newRow;
                validMoves[numValidMoves].col = newCol;
                numValidMoves++;
            }
        }
    } else {
        // Iterate through the move vectors
        for (int i = 0; i < numVectors; ++i) {
            // Get the current move vector
            MoveVector vector = vectors.vectors[i];
            // Compute the new position
            for (int j = 0; j < iter; ++j) {
                int newRow = pos.row + (vector.dy * direction * (j + 1));
                int newCol = pos.col + (vector.dx * direction * (j + 1));
                // Check if the new position is valid
                if (is_valid_position(newRow, newCol, piece.color, board)) {
                    if (piece.type == PAWN && board[newRow][newCol].type != EMPTY) {
                        break;
                    }
                    // Add the new position to the array of valid moves
                    Position newPos = {newRow, newCol};
                    validMoves[numValidMoves] = newPos;
                    // Increment the number of valid moves
                    numValidMoves++;
                    // If piece is of a different colour, then break out of the loop
                    if (board[newRow][newCol].type != EMPTY && board[newRow][newCol].color != piece.color) {
                        break;
                    }

                } else {
                    // Break out of the loop if the new position is not valid
                    break;
                }
            }
        }

        // Check for castling
        if (piece.type == KING) {
            // Check for castling on the king side
            if (kingMoved == 0 && rookMoved[1] == 0) {
                int valid = 1;
                for (int i = 1; i <= 2; ++i) {
                    if (board[pos.row][pos.col + i].type != EMPTY) {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    validMoves[numValidMoves].row = pos.row;
                    validMoves[numValidMoves].col = pos.col + 2;
                    numValidMoves++;
                }
            }
            // Check for castling on the queen side
            if (kingMoved == 0 && rookMoved[0] == 0) {
                int valid = 1;
                for (int i = 1; i <= 3; ++i) {
                    if (board[pos.row][pos.col - i].type != EMPTY) {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    validMoves[numValidMoves].row = pos.row;
                    validMoves[numValidMoves].col = pos.col - 2;
                    numValidMoves++;
                }
            }
        }
    }
    return validMoves;
}

MoveVectorList getMoveVectors(Piece piece) {
    // Return the move vectors for the given piece
    switch(piece.type) {
        case PAWN:
            return pawnMoveVectors;
        case KNIGHT:
            return knightMoveVectors;
        case BISHOP:
            return bishopMoveVectors;
        case ROOK:
            return rookMoveVectors;
        case QUEEN:
            return queenMoveVectors;
        case KING:
            return kingMoveVectors;
        default:
            return pawnMoveVectors;
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