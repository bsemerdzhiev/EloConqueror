#include "board.hpp"
#include "tree-search.hpp"

#include <iostream>
#include <string>

const std::string FEN_TO_USE =
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

int main() {
  Board board; //{FEN_TO_USE};

  std::string input;
  std::string square;
  while (true) {
    std::cin >> input;

    if (input == "move") {
      std::cin >> square;
      board = board.makeMove(square);
    } else if (input == "perft") {
      int32_t depth;
      std::cin >> depth;
      TreeSearch::search(board, depth);
    } else if (input == "display") {
      board.displayBoard();
    } else if (input == "fen") {
      std::getline(std::cin >> std::ws, square);
      board = Board{square};
    }
  }

  return 0;
}
