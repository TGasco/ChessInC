#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/move_vectors.h"
#include "../include/board.h"
#include "../include/movegen.h"
#include "../include/helperMethods.h"
// A comprehensive set of legal moves for each piece in Chess

// The following functions are used to determine the legal moves for each piece in Chess.
// The functions are called by the movePiece() function in board.c.

// Declare functions
void computeColourBitboard(int colour);
char colToFile(int col);
int getBitboardIndex(int pieceType, int colour);
int isCapture(uint64_t bitboard, int colour, int pieceType);
int isCheck(int colour);
int isCheckmate(int colour);
void updateAttackBitboards(int colour);
void updateGlobalAttackBitboards();
// int makeMove(Piece selectedPiece, int squareFrom, int squareTo, uint64_t* validMoves);
int makeMove(Piece selectedPiece, int squareFrom, int squareTo, uint64_t* validMoves, int simulate);
void validateMoves(int colour, uint64_t* validMoves);
void removePiece(uint64_t *bitboard, uint64_t bit);

void computeAllMoves(uint64_t* validMoves) {
    // Loop over all pieces of the given colour
    for (int colour = 0; colour < 2; colour++) {
        getPseudoValidMoves(colour, validMoves);
    }
    updateGlobalAttackBitboards();
}

void validateMoves(int colour, uint64_t* validMoves) {
    // Iterate over valid moves for each piece type
    for (int pieceType = 1; pieceType < 7; pieceType++) {
        int bitboardIndex = getBitboardIndex(pieceType, colour);
        uint64_t bitboard = bitboards[bitboardIndex];
        while (bitboard) {
            // Get the position of the piece
            int pos = __builtin_ctzll(bitboard);
            // Get the valid moves for the piece
            uint64_t moves = validMoves[pos];
            // Iterate over the valid moves
            while (moves) {
                // Get the position of the move
                int movePos = __builtin_ctzll(moves);
                // Make the move
                if (!makeMove((Piece){pieceType, colour}, pos, movePos, validMoves, 1)) {
                    // If the move puts the king in check, then remove the move from the valid moves
                    validMoves[pos] &= ~(1ULL << movePos);
                    // Remove from the attack bitboard
                    attackBitboards[bitboardIndex] &= ~(1ULL << movePos);
                    // printf("Invalid move: %d -> %d\n", pos, movePos);
                }
                // Remove the move from the valid moves
                moves &= ~(1ULL << movePos);
            }
            // Remove the piece from the bitboard
            bitboard &= ~(1ULL << pos);
        }
    }
}

uint64_t coordToBitboard(int row, int col) {
    // Convert the given coordinates to a bitboard
    // The bitboard is a 64-bit integer, where each bit represents a square on the board
    return 1ULL << (row * 8 + col);
}


Position* bitboardToPosition(uint64_t bitboard) {
    // Convert the given bitboard to a list of positions
    Position* positions = malloc(sizeof(Position) * (BOARD_SIZE * BOARD_SIZE));
    int numPositions = 0;
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        // Loop through the bitboard
        // If the bit at the current position is 1, then add the position to the list
        if (bitboard & (1ULL << i)) {
            Position pos = {i / BOARD_SIZE, i % BOARD_SIZE};
            positions[numPositions] = pos;
            numPositions++;
        }
    }
    positions[numPositions].row = -1;
    positions[numPositions].col = -1;
    return positions;
}


int isCapture(uint64_t bitboard, int colour, int pieceType) {
    // Check if the move is a capture
    uint64_t expr = bitboard & bitboards[colour ? 13 : 14];
    switch (pieceType)
    {
    case PAWN:
        expr = (bitboards[colour ? 13 : 14] | enPassantMask) & bitboard;
        break;
    default:
        break;
    }
    return expr != 0ULL;
}


int isCheck(int colour) {
    // Check if the given colour is in check
    // If the colour is in check, then return 1
    // Otherwise, return 0
    // print the attack bitboard if in check
    return (bitboards[colour ? 12 : 6] & attackBitboards[colour ? 13 : 14]) != 0;
}

int isCheckmate(int colour) {
    // If attack bitboard is empty, then return 1
    // Otherwise, return 0
    return (attackBitboards[colour ? 14 : 13] == 0ULL);
}

void removePiece(uint64_t *bitboard, uint64_t bit) {
    // Remove the piece from the bitboard
    *bitboard &= ~bit;
}

void promotePiece(uint64_t bit, int pieceType, int colour) {
    // Promote the piece at the given bit to the given piece type
    // Remove the piece from the bitboard
    removePiece(&bitboards[colour ? 13 : 14], bit);
    // Add the new piece to the bitboard
    bitboards[pieceType + (colour * 6)] |= bit;
}

void computeGlobalBitboard() {
    bitboards[0] = bitboards[13] | bitboards[14];
}

void computeColourBitboard(int colour) {
    switch (colour) {
        case WHITE:
            bitboards[13] = bitboards[1] | bitboards[2] | bitboards[3] | bitboards[4] | bitboards[5] | bitboards[6];
            break;
        case BLACK:
            bitboards[14] = bitboards[7] | bitboards[8] | bitboards[9] | bitboards[10] | bitboards[11] | bitboards[12];
            break;
    }
}

void updateAttackBitboards(int colour) {
    switch (colour)
    {
    case WHITE:
        attackBitboards[13] = attackBitboards[1] | attackBitboards[2] | attackBitboards[3] | attackBitboards[4] | attackBitboards[5] | attackBitboards[6];
        break;
    case BLACK:
        attackBitboards[14] = attackBitboards[7] | attackBitboards[8] | attackBitboards[9] | attackBitboards[10] | attackBitboards[11] | attackBitboards[12];
        break;
    
    default:
        break;
    }
}

void updateGlobalAttackBitboards() {
    attackBitboards[0] = attackBitboards[13] | attackBitboards[14];
}

void updateBitboards() {
    computeColourBitboard(WHITE);
    computeColourBitboard(BLACK);
    computeGlobalBitboard();
    updateAttackBitboards(WHITE);
    updateAttackBitboards(BLACK);
    updateGlobalAttackBitboards();
}

int getBitboardIndex(int pieceType, int colour) {
    return pieceType + (colour * 6);
}

int makeMove(Piece selectedPiece, int squareFrom, int squareTo, uint64_t* validMoves, int simulate) {
    uint64_t destBitboard = 1ULL << squareTo;
    int bitboardIndex = getBitboardIndex(selectedPiece.type, selectedPiece.color);

    // Check if the move is valid
    if (!(validMoves[squareFrom] & destBitboard)) {
        // printf("Invalid move!\n");
        return 0;
    }

    // Simulate the move to ensure that the king is not in check
    // Make a copy of the board state
    uint64_t tempBitboards[15];
    uint64_t tempAttackBitboards[15];
    memcpy(tempBitboards, bitboards, sizeof(bitboards));
    memcpy(tempAttackBitboards, attackBitboards, sizeof(attackBitboards));

    // Make the move


    // Remove the piece from the source square
    bitboards[bitboardIndex] &= ~(1ULL << squareFrom);

    // Handle Pawn promotion (if applicable)
    if (selectedPiece.type == PAWN && (destBitboard & promotionMask[selectedPiece.color]) != 0ULL) {
        // Promote the pawn by switching the piece type
        bitboardIndex = getBitboardIndex(QUEEN, selectedPiece.color);
    }

    // Move the piece to the destination square
    bitboards[bitboardIndex] |= (1ULL << squareTo);
    // Check if the move is a capture
    int capture = isCapture(destBitboard, selectedPiece.color, selectedPiece.type);
    if (capture) {
        printf("Capture!\n");
        // Get the piece at the destination
        if (selectedPiece.type == PAWN && ((1ULL << squareTo) & enPassantMask) != 0ULL) {
            removePiece(&bitboards[1 + (!(selectedPiece.color) * 6)], (1ULL << (squareTo + (selectedPiece.color ? -8 : 8))));
        } else {
            removePiece(&bitboards[getBoardAtIndex(squareTo, !selectedPiece.color)], destBitboard);
        }
    }

    // Update the bitboards to reflect the move
    updateBitboards();
    // Recompute attack bitboards for the opposite colour
    getPseudoValidMoves(!selectedPiece.color, validMoves);
    // Update the bitboards to reflect the attacks 
    updateBitboards();

    // Check if the move puts the king in check
    if (isCheck(selectedPiece.color)) {
        printf("Move puts king in check!\n");
        // If the move puts the king in check, then restore the board state and return 0
        memcpy(bitboards, tempBitboards, sizeof(bitboards));
        memcpy(attackBitboards, tempAttackBitboards, sizeof(attackBitboards));
        return 0;
    }

    // Undo the move if it is a simulation
    if (simulate) {
        memcpy(bitboards, tempBitboards, sizeof(bitboards));
        memcpy(attackBitboards, tempAttackBitboards, sizeof(attackBitboards));
        return 1;
    }


    // Set the en passant square (if applicable)
    if (selectedPiece.type == PAWN && abs(squareFrom - squareTo) == 16) {
        enPassantMask = 1ULL << (squareTo + (selectedPiece.color ? -8 : 8));
    } else {
        enPassantMask = 0ULL;
    }

    // Handle castling
    if (selectedPiece.type == KING) {
        // Move the rook
        if (abs(squareFrom - squareTo) == 2) {
            int rookSquareFrom = squareFrom + (squareTo - squareFrom > 0 ? 3 : -4);
            int rookSquareTo = squareFrom + (squareTo - squareFrom > 0 ? 1 : -1);
            // Move the rook
            bitboards[getBitboardIndex(ROOK, selectedPiece.color)] &= ~(1ULL << rookSquareFrom);
            bitboards[getBitboardIndex(ROOK, selectedPiece.color)] |= (1ULL << rookSquareTo);
        }

        // Redact the castling rights
        castleRights &= ~(selectedPiece.color ? BLACK_KINGSIDE : WHITE_KINGSIDE);
        castleRights &= ~(selectedPiece.color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE);
    }

    if (selectedPiece.type == ROOK && (castleRights & ((selectedPiece.color ? BLACK_KINGSIDE : WHITE_KINGSIDE))
        || (selectedPiece.color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE))) {
        // Check whether rook is king side or queen side
        int side = (squareFrom % 8 > 3) ? 0 : 1;

        // Redact the castling rights for the respective side
        castleRights &= ~(selectedPiece.color ? (side ? BLACK_QUEENSIDE : BLACK_KINGSIDE) : (side ? WHITE_QUEENSIDE : WHITE_KINGSIDE));
    }


    moveToNotation(selectedPiece, squareToPos(squareFrom), squareToPos(squareTo), capture);
    // Recompute attack bitboards
    getPseudoValidMoves(selectedPiece.color, validMoves);
    // updateAttackBitboards(selectedPiece.color);
    updateBitboards();
    return 1;
}

void parseFENToBitboard(char* fen) {
    // Parse the FEN string to a bitboard
    // The FEN string is stored in the global variable fen
    // The bitboard is stored in the global variable bitboard
    // The bitboard is a 64-bit integer, where each bit represents a square on the board
    // The least significant bit represents the square a1, and the most significant bit represents the square h8
    // The bitboard is initialized to 0

    int squareIndex = 56; // Start at a8 (the 56th index for a 64-bit integer)

    // Iterate through the FEN string
    while (*fen != '\0') {
        // If the current character is a digit, then shift the bitboard to the left by the given number of bits
        if (*fen >= '1' && *fen <= '8') {
            int shift = *fen - '0';
            squareIndex += shift;
            fen++;
            continue; // Skip the rest of the loop and continue with the next character
        } else if (*fen == '/') {
            squareIndex -= 16; // Skip to the next rank (8 squares in a rank)
        } else {
            // If the current character is a letter, then set the bit at the current position to 1
            int pieceIndex = -1;

            switch (*fen) {
                case 'p':
                    pieceIndex = PAWN + 6;
                    break;
                case 'P':
                    pieceIndex = PAWN;
                    break;
                case 'n':
                    pieceIndex = KNIGHT + 6;
                    break;
                case 'N':
                    pieceIndex = KNIGHT;
                    break;
                case 'b':
                    pieceIndex = BISHOP + 6;
                    break;
                case 'B':
                    pieceIndex = BISHOP;
                    break;
                case 'r':
                    pieceIndex = ROOK + 6;
                    break;
                case 'R':
                    pieceIndex = ROOK;
                    break;
                case 'q':
                    pieceIndex = QUEEN + 6;
                    break;
                case 'Q':
                    pieceIndex = QUEEN;
                    break;
                case 'k':
                    pieceIndex = KING + 6;
                    break;
                case 'K':
                    pieceIndex = KING;
                    break;
                default:
                    printf("Invalid piece: %c\n", *fen);
                    pieceIndex = -1;
                    break;
            }
        
            // Set the bit at the current position to 1 for the respective bitboard
            if (pieceIndex != -1) {
                uint64_t bitPosition = 1ULL << squareIndex;
                bitboards[pieceIndex] |= bitPosition;
            }
            squareIndex++;
        }
        // Increment the FEN string pointer
        fen++;
    }

    // Flip the vertical orientation of the bitboard
    // This is necessary because the FEN string starts at the 8th rank, whereas the bitboard starts at the 1st rank
    for (int i=1; i<13; i++) {
        verticalFlip(&bitboards[i]);
    }
    // Global bitboard is the bitwise OR of all the bitboards
    updateBitboards();
}