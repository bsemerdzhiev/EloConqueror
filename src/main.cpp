#include "evaluate.hpp"
#include "move-generator.hpp"
#include "uci.hpp"

#include <cassert>
#include <string>

const std::string FEN_TO_USE =
    "4rb1k/2pqn2p/6pn/ppp3N1/P1QP2b1/1P2p3/2B3PP/B3RRK1 w - - 0 24";
// ";
// "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

int main() {
  Evaluate::initTables();
  MoveGenerator::initAttackTables();

  UCI::run();
  // Move move = AlphaBeta::searchMove(board, 6);
  //
  // std::cout << Evaluate::evaluateBoard(board) << "\n";
  // UndoMove undo_move;
  // board.makeMove(move, undo_move);
  // std::cout << Evaluate::evaluateBoard(board) << "\n";
  //
  // std::cout << move.formatted() << "\n";
  // std::cout << "-------------------------\n";
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
