#include "alpha-beta.hpp"
#include "evaluate.hpp"
#include "move-generator.hpp"
#include "move.hpp"
#include "undo-move.hpp"
#include <cstdint>
#include <iostream>
#include <vector>

int32_t cnt = 0;
int32_t alphaBeta(Board &board, int32_t alpha, int32_t beta, int32_t depth,
                  std::vector<Move> all_moves[], Move &best_move,
                  bool should_change = false) {
  if (depth == 0) {
    cnt += 1;
    return Evaluate::evaluateBoard(board);
  }

  all_moves[depth].clear();
  MoveGenerator::searchAllMoves(board, board.getPlayerTurn(), all_moves[depth]);

  UndoMove undo_move;

  for (const auto &move : all_moves[depth]) {
    board.makeMove(move, undo_move);

    int32_t result =
        -alphaBeta(board, -beta, -alpha, depth - 1, all_moves, best_move);
    board.unmakeMove(undo_move);

    if (result >= beta) {
      return beta;
    }
    if (result > alpha) {
      alpha = result;
      if (should_change) {
        best_move = move;
      }
    }
  }

  return alpha;
}

Move AlphaBeta::searchMove(Board &board, int32_t depth) {
  std::vector<Move> all_moves[depth + 1];
  for (int32_t i = 0; i <= depth; i++) {
    all_moves[i].reserve(256);
  }

  Move best_move;

  alphaBeta(board, INT16_MIN, INT16_MAX, depth, all_moves, best_move, true);
  std::cout << cnt << "\n";

  return best_move;
}
