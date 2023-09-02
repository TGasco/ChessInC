#include <stdio.h>
#include <stdlib.h>
#include "../include/move_vectors.h"
#include "../include/board.h"
// A comprehensive set of legal moves for each piece in Chess

// The following functions are used to determine the legal moves for each piece in Chess.
// The functions are called by the movePiece() function in board.c.

// Declare functions
int is_valid_position(int row, int col, int colour, Piece board[BOARD_SIZE][BOARD_SIZE]);
Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE]);
MoveVectorList getMoveVectors(Piece piece);

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

Position* computeValidMoves(Position pos, Piece piece, Piece board[BOARD_SIZE][BOARD_SIZE]) {
    // Compute the valid moves for a piece at the given position on the given board
    // The valid moves are stored in the given array of valid moves
    // The number of valid moves is stored in the given integer pointer

    // Initialize the number of valid moves to 0
    int numValidMoves = 0;
    Position* validMoves = malloc(sizeof(Position) * 28);
    // Get the Move Vectors for the given piece
    MoveVectorList vectors = getMoveVectors(piece);
    int numVectors = vectors.count;
    // If Bishop, Rook or Queen, then the piece can move any number of squares in the given direction
    int iter = (piece.type == BISHOP || piece.type == ROOK || piece.type == QUEEN) ? 7 : 1;

    int direction = (piece.color == WHITE) ? 1 : -1;
    // Handle edge cases for Pawn moves
    if (piece.type == PAWN) {
        // Check if the Pawn can move two squares forward from its starting position
        if (pos.row == (piece.color == WHITE ? 1 : 6)) {
            Position newPos = {pos.row + (2 * direction), pos.col};
            if (is_valid_position(newPos.row, newPos.col, piece.color, board) &&
             board[newPos.row][newPos.col].type == EMPTY) {
                validMoves[numValidMoves] = newPos;
                numValidMoves++;
            }
        }
        // Check if the Pawn can capture a piece diagonally
        Position newPos = {pos.row + direction, pos.col + 1};
        if (is_valid_position(newPos.row, newPos.col, piece.color, board) && board[newPos.row][newPos.col].type != EMPTY) {
            validMoves[numValidMoves] = newPos;
            numValidMoves++;
        }
        newPos.col = pos.col - 1;
        if (is_valid_position(newPos.row, newPos.col, piece.color, board) && board[newPos.row][newPos.col].type != EMPTY) {
            validMoves[numValidMoves] = newPos;
            numValidMoves++;
        }
    }

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
    // // Print the list of valid moves (for debugging purposes)
    // if (numValidMoves > 0) {

    //     printf("Valid moves for %c at %d, %d: ", pieceToChar(piece), pos.col, pos.row);
    //     for (int i = 0; i < numValidMoves; ++i) {
    //         printf("{%d, %d}, ", validMoves[i].col, validMoves[i].row);
    //     }
    //     printf("\n");
    //     // Return the array of valid moves and the number of valid moves
    // } else {
    //     printf("No valid moves for %c at %d, %d\n", pieceToChar(piece), pos.col, pos.row);
    // }
    
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