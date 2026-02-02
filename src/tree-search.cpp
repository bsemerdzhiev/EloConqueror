#include "tree-search.hpp"
#include "search.hpp"
#include "undo_move.hpp"

uint64_t rec_search(Board &board, int32_t depth,
                    std::vector<Move> new_moves[]) {
  if (depth == 0) {
    return 1;
  }
  new_moves[depth].clear();
  MoveExplorer::searchAllMoves(board, board.getPlayerTurn(), new_moves[depth]);

  uint64_t cnt = 0;
  for (const auto &next_move : new_moves[depth]) {
    UndoMove undo_move;
    board.makeMove(next_move.pos_from, next_move.pos_to, next_move.piece_type,
                   board.getPlayerTurn(), next_move.move_type, undo_move);
    cnt += rec_search(board, depth - 1, new_moves);
    board.unmakeMove(undo_move);
  }
  return cnt;
}

uint64_t TreeSearch::search(Board &board, int32_t depth) {
  std::vector<Move> new_moves[10];
  for (int32_t i = 0; i < 10; i++) {
    new_moves[i].reserve(256);
  }
  return rec_search(board, depth, new_moves);
}
