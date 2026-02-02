#include "tree-search.hpp"
#include "search.hpp"
#include "undo_move.hpp"

uint64_t rec_search(Board &board, int32_t depth) {
  if (depth == 0) {
    return 1;
  }
  std::vector<Move> new_moves;
  MoveExplorer::searchAllMoves(board, board.getPlayerTurn(), new_moves);

  uint64_t cnt = 0;
  for (const auto &next_move : new_moves) {
    UndoMove undo_move;
    board.makeMove(next_move.pos_from, next_move.pos_to, next_move.piece_type,
                   board.getPlayerTurn(), next_move.move_type, undo_move);
    cnt += rec_search(board, depth - 1);
    board.unmakeMove(undo_move);
  }
  return cnt;
}

uint64_t TreeSearch::search(Board &board, int32_t depth) {

  return rec_search(board, depth);
}
