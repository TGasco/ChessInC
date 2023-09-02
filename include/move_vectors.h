// move_vectors.h
#ifndef MOVE_VECTORS_H
#define MOVE_VECTORS_H

typedef struct {
    int dx; // Column movement
    int dy; // Row movement
} MoveVector;

typedef struct {
    MoveVector* vectors;
    int count;
} MoveVectorList;

MoveVector pawnVectors[] = {
    {0, 1}, // Move forward
    // {0, 2}, // Initial two-step move
    // {-1, 1}, // Diagonal capture left
    // {1, 1}, // Diagonal capture right
};

MoveVector rookVectors[] = {
    {0, 1}, // Move up
    {0, -1}, // Move down
    {-1, 0}, // Move left
    {1, 0}, // Move right
};

MoveVector knightVectors[] = {
    {1, 2}, // Move up 2, right 1
    {-1, 2}, // Move up 2, left 1
    {1, -2}, // Move down 2, right 1
    {-1, -2}, // Move down 2, left 1
    {2, 1}, // Move up 1, right 2
    {-2, 1}, // Move up 1, left 2
    {2, -1}, // Move down 1, right 2
    {-2, -1}, // Move down 1, left 2
};

MoveVector bishopVectors[] = {
    {1, 1}, // Move up-right
    {-1, 1}, // Move up-left
    {1, -1}, // Move down-right
    {-1, -1}, // Move down-left
};

MoveVector queenVectors[] = {
    // Queen moves are the combination of rook and bishop moves
    {0, 1}, // Move up
    {0, -1}, // Move down
    {-1, 0}, // Move left
    {1, 0}, // Move right
    {1, 1}, // Move up-right
    {1, -1}, // Move up-left
    {-1, 1}, // Move down-right
    {-1, -1}, // Move down-left
};

MoveVector kingVectors[] = {
    {0, 1}, // Move up
    {0, -1}, // Move down
    {-1, 0}, // Move left
    {1, 0}, // Move right
    {1, 1}, // Move up-right
    {1, -1}, // Move up-left
    {-1, 1}, // Move down-right
    {-1, -1}, // Move down-left
};

MoveVectorList pawnMoveVectors = {
    .vectors = pawnVectors,
    .count = 4
};

MoveVectorList rookMoveVectors = {
    .vectors = rookVectors,
    .count = 4
};

MoveVectorList knightMoveVectors = {
    .vectors = knightVectors,
    .count = 8
};

MoveVectorList bishopMoveVectors = {
    .vectors = bishopVectors,
    .count = 4
};

MoveVectorList queenMoveVectors = {
    .vectors = queenVectors,
    .count = 8
};

MoveVectorList kingMoveVectors = {
    .vectors = kingVectors,
    .count = 8
};


#endif