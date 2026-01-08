#include "tree-search.hpp"
#include "search.hpp"

#include <iostream>
#include <map>
#include <queue>
#include <tuple>

int64_t TreeSearch::search(const Board &board, int32_t depth) {
  std::queue<std::tuple<Board, int32_t, std::string>> all_boards;

  all_boards.push(std::make_tuple(board, 0, " "));

  int64_t cnt = 0;

  std::map<std::string, int64_t> move_counts;

  while (!all_boards.empty()) {
    std::tuple<Board, int32_t, std::string> head_node = all_boards.front();
    all_boards.pop();

    const Board &current_node = std::get<0>(head_node);
    const int32_t cur_depth = std::get<1>(head_node);
    const std::string &initial_move = std::get<2>(head_node);

    if (cur_depth == depth) {
      cnt++;
      move_counts[initial_move]++;
      continue;
    }

    std::vector<Move> moves = MoveExplorer::searchAllMoves(
        current_node, current_node.getPlayerTurn(), true);
    for (const auto &next_move : moves) {
      all_boards.push(std::make_tuple(
          current_node.makeMove(
              next_move.pos_from, next_move.pos_to, next_move.piece_type,
              current_node.getPlayerTurn(), next_move.move_type),
          cur_depth + 1,
          initial_move == " " ? next_move.formatted() : initial_move));
    }
  }

  /*for (const auto &[x1, x2] : move_counts) {
    std::cout << x1 << ": " << x2 << std::endl;
  }
  std::cout << cnt << std::endl;*/
  return cnt;
}
