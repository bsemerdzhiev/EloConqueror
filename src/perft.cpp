#include "perft.hpp"
#include "board_inl.hpp"
#include "evaluate.hpp"
#include "move-generator.hpp"
#include "undo-move.hpp"
#include "util.hpp"

#include <chrono>
#include <format>
#include <iostream>

using clk = std::chrono::high_resolution_clock;

int64_t Perft::search(Board &board, int32_t depth) {
  std::vector<Move> new_moves[10];
  int32_t visited[10] = {0};
  for (int32_t i = 0; i < 10; i++) {
    new_moves[i].reserve(256);
  }

  clk::time_point start = clk::now();

  int64_t cnt = 0;
  int64_t partial_cnt = 0;

  int32_t cur_depth = depth;
  std::array<UndoMove, 10> undo_moves;
  std::array<uint64_t, 10> king_positions;

  std::string first_move = "";

  MoveGenerator::generatePseudoLegalMoves(board, new_moves[depth]);
  bool cur_turn = board.getPlayerTurn();

  uint64_t king_pos = board.getPiece(Pieces::KING, board.getPlayerTurn() ^ 1);

  while (true) {
    if (BoardInl::cellIsUnderAttack(board, king_pos, cur_turn ^ 1)) {
      cur_depth++;
      cur_turn ^= 1;
      board.unmakeMove(undo_moves[cur_depth]);

      king_pos = king_positions[cur_depth];
      continue;
    }
    if (cur_depth == 0) {
      // final_cnt[first_move]++;
      partial_cnt += 1;
      cur_depth++;
      cur_turn ^= 1;

      board.unmakeMove(undo_moves[cur_depth]);
      king_pos = king_positions[cur_depth];

      continue;
    } else if (visited[cur_depth] == new_moves[cur_depth].size()) {
      cur_depth++;
      cur_turn ^= 1;
      if (cur_depth > depth) {
        break;
      }

      board.unmakeMove(undo_moves[cur_depth]);
      king_pos = king_positions[cur_depth];

      continue;
    }

    const Move &move_to_make = new_moves[cur_depth][visited[cur_depth]];

    if (cur_depth == depth) {
      if (first_move != "") {
        cnt += partial_cnt;

        std::cout << std::format("{}: {}\n", first_move, partial_cnt);

        partial_cnt = 0;
      }
      first_move = move_to_make.formatted();
    }

    visited[cur_depth] += 1;

    king_positions[cur_depth] = king_pos;
    king_pos = board.makeMove(move_to_make, undo_moves[cur_depth]);

    if (cur_depth - 1 > 0) {
      new_moves[cur_depth - 1].clear();
      MoveGenerator::generatePseudoLegalMoves(board, new_moves[cur_depth - 1]);
      visited[cur_depth - 1] = 0;
    }
    cur_depth--;
    cur_turn ^= 1;
  }

  {
    cnt += partial_cnt;

    std::cout << std::format("{}: {}\n", first_move, partial_cnt);
    partial_cnt = 0;
  }

  clk::time_point end = clk::now();

  std::cout << "\n";

  std::cout << std::format("Nodes searched: {}\n", cnt);

  int64_t ms =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();

  std::cout << std::format("NPS: {}\n", 1'000'000.0 * (1.0 * cnt / ms));

  return cnt;
}
