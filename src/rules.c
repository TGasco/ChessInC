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
void updateBitboards();
void computeColourBitboard(int colour);
char colToFile(int col);
int getBitboardIndex(int pieceType, int colour);
int isCapture(uint64_t bitboard, int colour, int pieceType);
int isCheck(int colour);
int isCheckmate(int colour);
void updateAttackBitboards(int colour);
void updateGlobalAttackBitboards();
// int makeMove(Piece selectedPiece, int squareFrom, int squareTo, uint64_t* validMoves);
int makeMove(Move move, int simulate);
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
        uint64_t bitboard = currentState.bitboards[bitboardIndex];
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
                Move move = {
                    .piece = (Piece){pieceType, colour},
                    .from = pos,
                    .to = movePos
                };
                if (!makeMove(move, 1)) {
                // if (!makeMove((Piece){pieceType, colour}, pos, movePos, validMoves, 1)) {
                    // If the move puts the king in check, then remove the move from the valid moves
                    validMoves[pos] &= ~(1ULL << movePos);
                    // Remove from the attack bitboard
                    currentState.attackBitboards[bitboardIndex] &= ~(1ULL << movePos);
                    if (pieceType == PAWN) {
                        // Remove pawn captures from the attack bitboard
                        currentState.attackBitboards[bitboardIndex] &= ~(getPawnCaptureMask(1ULL << pos, colour));
                    }
                    saveBoardState(&prevState);
                    // printf("Invalid move: %d -> %d\n", pos, movePos);
                }
                // Remove the move from the valid moves
                moves &= ~(1ULL << movePos);
            }
            // Remove the piece from the bitboard
            bitboard &= ~(1ULL << pos);
        }
    }
    updateBitboards();
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
    uint64_t expr = bitboard & currentState.bitboards[colour ? 13 : 14];
    switch (pieceType)
    {
    case PAWN:
        expr = (currentState.bitboards[colour ? 13 : 14] | currentState.enPassantMask) & bitboard;
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
    return (currentState.bitboards[colour ? 12 : 6] & currentState.attackBitboards[colour ? 13 : 14]) != 0;
}

int isCheckmate(int colour) {
    // If attack bitboard is empty, then return 1
    // Otherwise, return 0
    return (currentState.attackBitboards[colour == WHITE ? WHITE_GLOB : BLACK_GLOB] == 0ULL);
}

int isEndGame() {
    // Check if the game is in the endgame
    // The game is in the endgame if either there are no queens on the board, or if the total material (excluding kings) is less than a certain threshold
    int threshold = 6;
    return (currentState.bitboards[WHITE_QUEEN] == 0ULL && currentState.bitboards[BLACK_QUEEN] == 0ULL) || (__builtin_popcount(currentState.bitboards[GLOBAL]) < threshold);
}

void removePiece(uint64_t *bitboard, uint64_t bit) {
    // Remove the piece from the bitboard
    *bitboard &= ~bit;
}

void promotePiece(uint64_t bit, int pieceType, int colour) {
    // Promote the piece at the given bit to the given piece type
    // Remove the piece from the bitboard
    removePiece(&currentState.bitboards[colour ? 13 : 14], bit);
    // Add the new piece to the bitboard
    currentState.bitboards[pieceType + (colour * 6)] |= bit;
}

void computeGlobalBitboard() {
    currentState.bitboards[GLOBAL] = currentState.bitboards[WHITE_GLOB] | currentState.bitboards[BLACK_GLOB];
}

void computeColourBitboard(int colour) {
    switch (colour) {
        case WHITE:
            currentState.bitboards[WHITE_GLOB] = currentState.bitboards[1] | currentState.bitboards[2] | currentState.bitboards[3] | currentState.bitboards[4] | currentState.bitboards[5] | currentState.bitboards[6];
            break;
        case BLACK:
            currentState.bitboards[BLACK_GLOB] = currentState.bitboards[7] | currentState.bitboards[8] | currentState.bitboards[9] | currentState.bitboards[10] | currentState.bitboards[11] | currentState.bitboards[12];
            break;
    }
}

void updateAttackBitboards(int colour) {
    switch (colour)
    {
    case WHITE:
        currentState.attackBitboards[WHITE_GLOB] = currentState.attackBitboards[1] | currentState.attackBitboards[2] | currentState.attackBitboards[3] | currentState.attackBitboards[4] | currentState.attackBitboards[5] | currentState.attackBitboards[6];
        break;
    case BLACK:
        currentState.attackBitboards[BLACK_GLOB] = currentState.attackBitboards[7] | currentState.attackBitboards[8] | currentState.attackBitboards[9] | currentState.attackBitboards[10] | currentState.attackBitboards[11] | currentState.attackBitboards[12];
        break;
    
    default:
        break;
    }
}

void updateGlobalAttackBitboards() {
    currentState.attackBitboards[GLOBAL] = currentState.attackBitboards[WHITE_GLOB] | currentState.attackBitboards[BLACK_GLOB];
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

int isMoveValid(Move move, uint64_t* validMoves) {
    // Check if the move is valid
    return validMoves[move.from] & (1ULL << move.to) ? 1 : 0;
}

int countValidMoves(uint64_t* validMoves) {
    // Count the number of valid moves
    int count = 0;
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        count += __builtin_popcountll(validMoves[i]);
    }
    return count;
}

int makeMove(Move move, int simulate) {
    uint64_t destBitboard = 1ULL << move.to;
    uint64_t* tmpMoves = malloc(sizeof(uint64_t) * BOARD_SIZE * BOARD_SIZE);
    int bitboardIndex = getBitboardIndex(move.piece.type, move.piece.color);

    // Remove the piece from the source square
    currentState.bitboards[bitboardIndex] &= ~(1ULL << move.from);

    // Handle Pawn promotion (if applicable)
    if (move.piece.type == PAWN && (destBitboard & promotionMask[move.piece.color]) != 0ULL) {
        // Promote the pawn by switching the piece type
        bitboardIndex = getBitboardIndex(QUEEN, move.piece.color);
    }

    // Check if the move is a capture
    int capture = isCapture(destBitboard, move.piece.color, move.piece.type);
    if (capture) {
        // Get the piece at the destination
        if (move.piece.type == PAWN && ((1ULL << move.to) & currentState.enPassantMask) != 0ULL) {
            removePiece(&currentState.bitboards[1 + (!(move.piece.color) * 6)], (1ULL << (move.to + (move.piece.color ? -8 : 8))));
        } else {
            removePiece(&currentState.bitboards[getBoardAtIndex(move.to, !move.piece.color)], destBitboard);
        }
    }

    // Move the piece to the destination square
    currentState.bitboards[bitboardIndex] |= (1ULL << move.to);

    // Update the bitboards to reflect the move
    updateBitboards();

    if (simulate) {
        // Recompute attack bitboards for the opposite colour
        getPseudoValidMoves(!move.piece.color, tmpMoves);
        // Update the bitboards to reflect the attacks 
        updateBitboards();
        int check = isCheck(move.piece.color);
        free(tmpMoves);

        restoreBoardState(&prevState); // restore the board state to the previous state
        return !check;
    }

    // Set the en passant square (if applicable)
    if (move.piece.type == PAWN && abs(move.from - move.to) == 16) {
        currentState.enPassantMask = 1ULL << (move.to + (move.piece.color ? -8 : 8));
    } else {
        currentState.enPassantMask = 0ULL;
    }

    // Handle castling
    if (move.piece.type == KING) {
        // Move the rook
        if (abs(move.from - move.to) == 2) {
            int rookSquareFrom = move.from + (move.to - move.from > 0 ? 3 : -4);
            int rookSquareTo = move.from + (move.to - move.from > 0 ? 1 : -1);
            // Move the rook
            currentState.bitboards[getBitboardIndex(ROOK, move.piece.color)] &= ~(1ULL << rookSquareFrom);
            currentState.bitboards[getBitboardIndex(ROOK, move.piece.color)] |= (1ULL << rookSquareTo);
        }

        // Redact the castling rights
        currentState.castleRights &= ~(move.piece.color ? BLACK_KINGSIDE : WHITE_KINGSIDE);
        currentState.castleRights &= ~(move.piece.color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE);
    }

    if (move.piece.type == ROOK && (currentState.castleRights & ((move.piece.color ? BLACK_KINGSIDE : WHITE_KINGSIDE))
        || (move.piece.color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE))) {
        // Check whether rook is king side or queen side
        int side = (move.from % 8 > 3) ? 0 : 1;

        // Redact the castling rights for the respective side
        currentState.castleRights &= ~(move.piece.color ? (side ? BLACK_QUEENSIDE : BLACK_KINGSIDE) : (side ? WHITE_QUEENSIDE : WHITE_KINGSIDE));
    }


    // moveToNotation(move.piece, squareToPos(move.from), squareToPos(move.to), capture);
    free(tmpMoves);
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
                currentState.bitboards[pieceIndex] |= bitPosition;
            }
            squareIndex++;
        }
        // Increment the FEN string pointer
        fen++;
    }

    // Flip the vertical orientation of the bitboard
    // This is necessary because the FEN string starts at the 8th rank, whereas the bitboard starts at the 1st rank
    for (int i=1; i<13; i++) {
        verticalFlip(&currentState.bitboards[i]);
    }
    // Global bitboard is the bitwise OR of all the bitboards
    updateBitboards();
}