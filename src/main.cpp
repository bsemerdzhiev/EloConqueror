#include "alpha-beta.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "move-generator.hpp"
#include "move.hpp"
#include "undo-move.hpp"

#include <iostream>
#include <string>

const std::string FEN_TO_USE =
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
// ";
// "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

int main() {
  Evaluate::initTables();
  MoveGenerator::initAttackTables();

  // std::cout << Perft::search(board, 5) << std::endl;
  Board board{FEN_TO_USE};
  Move move = AlphaBeta::searchMove(board, 6);

  std::cout << Evaluate::evaluateBoard(board) << "\n";
  UndoMove undo_move;
  board.makeMove(move, undo_move);
  std::cout << Evaluate::evaluateBoard(board) << "\n";

  std::cout << move.formatted() << "\n";
  std::cout << "-------------------------\n";
  /*
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
  */
  return 0;
}
