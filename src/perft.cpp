#include "perft.hpp"
#include "evaluate.hpp"
#include "move-generator.hpp"
#include "undo-move.hpp"
#include <format>
#include <iostream>
#include <unordered_map>

int64_t Perft::search(Board &board, int32_t depth) {
  std::vector<Move> new_moves[10];
  int32_t visited[10] = {0};
  for (int32_t i = 0; i < 10; i++) {
    new_moves[i].reserve(256);
  }

  int64_t cnt = 0;
  int32_t cur_depth = depth;
  std::array<UndoMove, 10> undo_moves;

  std::unordered_map<std::string, int32_t> final_cnt;
  std::string first_move;

  MoveGenerator::searchAllMoves(board, board.getPlayerTurn(), new_moves[depth]);
  while (true) {
    if (cur_depth == 0) {
      final_cnt[first_move]++;
      cnt += 1;
      cur_depth++;

      board.unmakeMove(undo_moves[cur_depth]);

      continue;
    } else if (visited[cur_depth] == new_moves[cur_depth].size()) {
      cur_depth++;
      if (cur_depth > depth) {
        break;
      }

      board.unmakeMove(undo_moves[cur_depth]);

      continue;
    }

    const Move &move_to_make = new_moves[cur_depth][visited[cur_depth]];

    if (cur_depth == depth) {
      first_move = move_to_make.formatted();
    }

    visited[cur_depth] += 1;

    board.makeMove(move_to_make, undo_moves[cur_depth]);

    if (cur_depth - 1 > 0) {
      new_moves[cur_depth - 1].clear();
      MoveGenerator::searchAllMoves(board, board.getPlayerTurn(),
                                    new_moves[cur_depth - 1]);
      visited[cur_depth - 1] = 0;
    }
    cur_depth--;
  }

  for (const auto &key_value : final_cnt) {
    std::cout << std::format("{}: {}\n", key_value.first, key_value.second);
  }
  std::cout << "\n";

  std::cout << std::format("Nodes searched: {}\n", cnt);

  return cnt;
}
