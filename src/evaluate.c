// #include <limits.h>
// #include <stdbool.h>

// #define INFINITY INT_MAX

// int evaluate_board(); // Forward declaration of the evaluation function
// void make_move(Move move); // Function to make a move
// void undo_move(Move move); // Function to undo a move
// bool game_over(); // Function to check if the game is over
// Move* generate_moves(bool maximizingPlayer, int* move_count); // Function to generate all possible moves for the current player

// int minimax(int depth, int alpha, int beta, bool maximizingPlayer) {
//     if (depth == 0 || game_over()) {
//         return evaluate_board();
//     }

//     if (maximizingPlayer) {
//         int maxEval = -INFINITY;
//         int move_count;
//         Move* moves = generate_moves(true, &move_count);
//         for (int i = 0; i < move_count; i++) {
//             make_move(moves[i]);
//             int eval = minimax(depth - 1, alpha, beta, false);
//             undo_move(moves[i]);
//             maxEval = max(maxEval, eval);
//             alpha = max(alpha, eval);
//             if (beta <= alpha) {
//                 break; // beta cut-off
//             }
//         }
//         free(moves); // Assuming moves were dynamically allocated
//         return maxEval;
//     } else {
//         int minEval = INFINITY;
//         int move_count;
//         Move* moves = generate_moves(false, &move_count);
//         for (int i = 0; i < move_count; i++) {
//             make_move(moves[i]);
//             int eval = minimax(depth - 1, alpha, beta, true);
//             undo_move(moves[i]);
//             minEval = min(minEval, eval);
//             beta = min(beta, eval);
//             if (beta <= alpha) {
//                 break; // alpha cut-off
//             }
//         }
//         free(moves); // Assuming moves were dynamically allocated
//         return minEval;
//     }
// }

// // Example max and min functions
// int max(int a, int b) {
//     return (a > b) ? a : b;
// }

// int min(int a, int b) {
//     return (a < b) ? a : b;
// }

// int evaluate_board() {
//     int score = 0;

//     // Assuming you have bitboards for each piece type and color
//     // Example: bitboard for white pawns, black pawns, etc.
//     score += __builtin_popcountll(white_pawns) * 1;
//     score -= __builtin_popcountll(black_pawns) * 1;

//     score += __builtin_popcountll(white_knights) * 3;
//     score -= __builtin_popcountll(black_knights) * 3;

//     score += __builtin_popcountll(white_bishops) * 3;
//     score -= __builtin_popcountll(black_bishops) * 3;

//     score += __builtin_popcountll(white_rooks) * 5;
//     score -= __builtin_popcountll(black_rooks) * 5;

//     score += __builtin_popcountll(white_queens) * 9;
//     score -= __builtin_popcountll(black_queens) * 9;

//     // No need to count kings for evaluation in a typical material evaluation function
//     return score;
// }

// Move find_best_move(int depth, bool maximizingPlayer) {
//     int bestValue = maximizingPlayer ? -INFINITY : INFINITY;
//     Move bestMove;
//     int move_count;
//     Move* moves = generate_moves(maximizingPlayer, &move_count);

//     for (int i = 0; i < move_count; i++) {
//         make_move(moves[i]);
//         int boardValue = minimax(depth - 1, -INFINITY, INFINITY, !maximizingPlayer);
//         undo_move(moves[i]);

//         if (maximizingPlayer && boardValue > bestValue) {
//             bestValue = boardValue;
//             bestMove = moves[i];
//         } else if (!maximizingPlayer && boardValue < bestValue) {
//             bestValue = boardValue;
//             bestMove = moves[i];
//         }
//     }
//     free(moves); // Assuming moves were dynamically allocated
//     return bestMove;
// }
