#include "alpha-beta.hpp"
#include "board-inl.hpp"
#include "evaluate.hpp"
#include "move-generator.hpp"
#include "move.hpp"
#include "undo-move.hpp"

#include <cstdint>
#include <format>
#include <iostream>
#include <vector>

int32_t alphaBeta(Board &board, int32_t alpha, int32_t beta, int32_t depth,
                  std::vector<Move> all_moves[], Move &best_move,
                  bool player_turn, bool should_change = false) {
  if (depth == 0) {
    return Evaluate::evaluateBoard(board);
  }

  all_moves[depth].clear();
  MoveGenerator::generatePseudoLegalMoves(board, all_moves[depth]);

  UndoMove undo_move;

  uint64_t king_pos;
  for (const auto &move : all_moves[depth]) {
    king_pos = board.makeMove(move, undo_move);

    if (BoardInl::cellIsUnderAttack(board, king_pos, player_turn)) {
      board.unmakeMove(undo_move);
      continue;
    }

    int32_t result = -alphaBeta(board, -beta, -alpha, depth - 1, all_moves,
                                best_move, player_turn ^ 1);
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

  if (all_moves[depth].empty()) {
    return -10000;
  }

  return alpha;
}

void AlphaBeta::searchMove(Board &board, int32_t depth) {
  std::vector<Move> all_moves[depth + 1];
  for (int32_t i = 0; i <= depth; i++) {
    all_moves[i].reserve(256);
  }

  Move best_move;

  alphaBeta(board, INT16_MIN, INT16_MAX, depth, all_moves, best_move,
            board.getPlayerTurn(), true);

  std::cout << std::format("bestmove {}\n", best_move.formatted());
}
