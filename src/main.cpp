#include "evaluate.hpp"
#include "move-generator.hpp"
#include "uci.hpp"

#include <cassert>
#include <iostream>
#include <string>

const std::string FEN_TO_USE =
    "4rb1k/2pqn2p/6pn/ppp3N1/P1QP2b1/1P2p3/2B3PP/B3RRK1 w - - 0 24";

int main() {
  Evaluate::initTables();
  MoveGenerator::initAttackTables();

  UCI::run();

  // Board board{
  // "r2q1rk1/ppp2ppp/2np1n2/4p3/2B1P3/2NP1N2/PPP2PPP/R1BQ1RK1 w - - 0 8"};

  // Evaluate::initTables();
  // double evaluation = Evaluate::evaluateBoard(board);
  // std::cout << evaluation << "\n";

  return 0;
}
