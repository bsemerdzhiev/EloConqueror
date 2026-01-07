#include "board.hpp"
#include "search.hpp"

#include <iostream>
#include <vector>

int main() {
  Board board;

  std::vector<Move> moves =
      MoveExplorer::searchAllMoves(board, board.getPlayerTurn(), true);

  for (const auto &new_move : moves) {
    std::cout << new_move.pos_from << " " << new_move.pos_to << std::endl;
  }

  return 0;
}
