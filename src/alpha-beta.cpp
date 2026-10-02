#include "alpha-beta.hpp"
#include "board-inl.hpp"
#include "evaluate.hpp"
#include "move-generator.hpp"
#include "move.hpp"
#include "polyglot-book.hpp"
#include "transposition-table.hpp"
#include "undo-move.hpp"

#include <cassert>
#include <cstdint>
#include <format>
#include <iostream>
#include <vector>

ScoreType quiesce(Board &board, int32_t alpha, int32_t beta,
                  std::vector<std::vector<Move>> &capture_moves,
                  bool player_turn, int32_t cur_ply, int32_t moves_made);

constexpr ScoreType LOSS_SCORE = -10000;
constexpr ScoreType INF = 1000000;

ScoreType alphaBeta(Board &board, ScoreType alpha, ScoreType beta,
                    std::size_t ply, std::vector<std::vector<Move>> &all_moves,
                    std::vector<std::vector<Move>> &capture_moves,
                    bool player_turn, int32_t moves_made = 0) {
  // check the transposition table first
  const TranspositionTableEntry *tt_entry = TranspositionTable::find(board);

  const Move *pv_move = nullptr;

  // check if we will cut-off
  if (tt_entry != nullptr) {
    pv_move = &tt_entry->best_move;

    if (tt_entry->depth >= ply) {
      if (tt_entry->node_type == NodeType::Exact) {
        return tt_entry->score;
      } else if (tt_entry->node_type == NodeType::LowerBound &&
                 tt_entry->score >= beta) {
        return beta;
      } else if (tt_entry->node_type == NodeType::UpperBound &&
                 tt_entry->score <= alpha) {
        return alpha;
      }
    }
  }

  all_moves[ply].clear();
  MoveGenerator::generatePseudoLegalMoves(board, all_moves[ply]);

  UndoMove undo_move;

  uint64_t king_pos;

  bool has_made_a_legal_move = false;
  bool has_best_move = false;

  const ScoreType original_alpha = alpha;

  Move best_move;

  auto it = all_moves[ply].begin();

  while (it != all_moves[ply].end()) {
    it = getBestMove(all_moves[ply], it, pv_move);

    const Move &move = *it;

    king_pos = board.makeMove(move, undo_move);

    if (BoardInl::cellIsUnderAttack(board, king_pos, player_turn)) {
      board.unmakeMove(undo_move);
      it = next(it);
      continue;
    }

    has_made_a_legal_move = true;

    if (ply == 0) {
      board.unmakeMove(undo_move);
      it = next(it);
      continue;
    }

    ScoreType result =
        -alphaBeta(board, -beta, -alpha, ply - 1, all_moves, capture_moves,
                   player_turn ^ 1, moves_made + 1);
    board.unmakeMove(undo_move);

    if (result >= beta) {
      TranspositionTable::insert(board, move, ply, result,
                                 NodeType::LowerBound);
      return beta;
    }
    if (result > alpha) {
      has_best_move = true;
      best_move = move;
      alpha = result;
    }

    it = next(it);
  }

  if (!has_made_a_legal_move) {
    if (BoardInl::kingIsUnderAttack(board, player_turn)) {
      alpha = LOSS_SCORE + moves_made;
    } else {
      alpha = 0;
    }
  } else {
    if (ply == 0) {
      return quiesce(board, alpha, beta, capture_moves, player_turn, 0,
                     moves_made);
    }

    if (has_best_move) {
      TranspositionTable::insert(
          board, best_move, ply, alpha,
          (alpha > original_alpha) ? NodeType::Exact : NodeType::UpperBound);
    }
  }

  return alpha;
}

int32_t quiesce(Board &board, int32_t alpha, int32_t beta,
                std::vector<std::vector<Move>> &capture_moves, bool player_turn,
                int32_t cur_ply, int32_t moves_made) {

  bool is_in_check = BoardInl::kingIsUnderAttack(board, board._player_turn);

  if (!is_in_check) {
    int32_t static_eval = Evaluate::evaluateBoard(board);

    if (static_eval >= beta) {
      return beta;
    }
    if (static_eval > alpha) {
      alpha = static_eval;
    }
  }

  capture_moves[cur_ply].clear();
  MoveGenerator::generatePseudoLegalMoves(board, capture_moves[cur_ply]);

  uint64_t king_pos;
  UndoMove undo_move;

  bool has_made_a_legal_move = false;

  for (auto &move : capture_moves[cur_ply]) {
    // make sure the move is a capture
    if (!is_in_check && !move.captures) {
      continue;
    }
    king_pos = board.makeMove(move, undo_move);

    if (BoardInl::cellIsUnderAttack(board, king_pos, player_turn)) {
      board.unmakeMove(undo_move);
      continue;
    }

    has_made_a_legal_move = true;

    int32_t result = -quiesce(board, -beta, -alpha, capture_moves,
                              player_turn ^ 1, cur_ply + 1, moves_made + 1);

    board.unmakeMove(undo_move);

    if (result >= beta) {
      return beta;
    }
    if (result > alpha) {
      alpha = result;
    }
  }

  if (is_in_check && !has_made_a_legal_move) {
    alpha = LOSS_SCORE + moves_made;
  }

  return alpha;
}

void AlphaBeta::iterativeDeepening(Board &board, std::size_t ply) {
  Move best_move = polyglot.look_up(board);

  if (best_move.pos_from != 0) {
    std::cout << std::format("bestmove {}\n", best_move.formatted());
    return;
  }

  constexpr static int32_t CAPTURE_CHAIN = 80;

  std::vector<std::vector<Move>> all_moves(ply + 1);
  std::vector<std::vector<Move>> capture_moves(CAPTURE_CHAIN);

  for (std::size_t i{0}; i <= ply; i++) {
    all_moves[i].reserve(256);
  }

  for (std::size_t i{0}; i < CAPTURE_CHAIN; i++) {
    capture_moves[i].reserve(256);
  }

  for (std::size_t i{1}; i <= ply; i++) {
    alphaBeta(board, -INF, INF, i, all_moves, capture_moves,
              board.getPlayerTurn());
  }

  const TranspositionTableEntry *entry = TranspositionTable::find(board);

  std::cout << std::format("bestmove {}\n", entry->best_move.formatted());
}
