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

int32_t quiesce(Board &board, int32_t alpha, int32_t beta,
                std::vector<Move> capture_moves[], bool player_turn,
                int32_t cur_depth);

int32_t alphaBeta(Board &board, int32_t alpha, int32_t beta, int32_t depth,
                  std::vector<Move> all_moves[],
                  std::vector<Move> capture_moves[], Move &best_move,
                  bool player_turn, bool should_change = false) {
  if (depth == 0) {
    return quiesce(board, alpha, beta, capture_moves, player_turn, 0);
  }

  all_moves[depth].clear();
  MoveGenerator::generatePseudoLegalMoves(board, all_moves[depth]);

  UndoMove undo_move;

  uint64_t king_pos;
  int32_t legal_moves_cnt = 0;

  for (const auto &move : all_moves[depth]) {
    king_pos = board.makeMove(move, undo_move);

    if (BoardInl::cellIsUnderAttack(board, king_pos, player_turn)) {
      board.unmakeMove(undo_move);
      continue;
    }

    legal_moves_cnt++;

    int32_t result = -alphaBeta(board, -beta, -alpha, depth - 1, all_moves,
                                capture_moves, best_move, player_turn ^ 1);
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

  if (legal_moves_cnt == 0) {
    return -10000;
  }

  return alpha;
}

int32_t quiesce(Board &board, int32_t alpha, int32_t beta,
                std::vector<Move> capture_moves[], bool player_turn,
                int32_t cur_depth) {
  int32_t static_eval = Evaluate::evaluateBoard(board);

  if (static_eval >= beta) {
    return beta;
  }
  if (static_eval > alpha) {
    alpha = static_eval;
  }

  capture_moves[cur_depth].clear();
  MoveGenerator::generatePseudoLegalMoves(board, capture_moves[cur_depth]);

  uint64_t king_pos;
  UndoMove undo_move;
  for (auto &move : capture_moves[cur_depth]) {

    // make sure the move is a capture
    if (!move.captures) {
      continue;
    }
    king_pos = board.makeMove(move, undo_move);

    if (BoardInl::cellIsUnderAttack(board, king_pos, player_turn)) {
      board.unmakeMove(undo_move);
      continue;
    }

    int32_t result = -quiesce(board, -beta, -alpha, capture_moves,
                              player_turn ^ 1, cur_depth + 1);

    board.unmakeMove(undo_move);

    if (result >= beta) {
      return beta;
    }
    if (result > alpha) {
      alpha = result;
    }
  }

  return alpha;
}

void AlphaBeta::searchMove(Board &board, int32_t depth) {
  constexpr int32_t CAPTURE_CHAIN = 40;

  std::vector<Move> all_moves[depth + 1];
  std::vector<Move> capture_moves[CAPTURE_CHAIN];

  for (int32_t i = 0; i <= depth; i++) {
    all_moves[i].reserve(256);
  }

  for (int32_t i = 0; i < CAPTURE_CHAIN; i++) {
    capture_moves[i].reserve(256);
  }

  Move best_move;

  alphaBeta(board, INT16_MIN, INT16_MAX, depth, all_moves, capture_moves,
            best_move, board.getPlayerTurn(), true);

  std::cout << std::format("bestmove {}\n", best_move.formatted());
}
