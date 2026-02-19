#include "evaluate.hpp"
#include "move-generator.hpp"
#include "uci.hpp"

#include <cassert>
#include <string>

const std::string FEN_TO_USE =
    "4rb1k/2pqn2p/6pn/ppp3N1/P1QP2b1/1P2p3/2B3PP/B3RRK1 w - - 0 24";

int main() {
  Evaluate::initTables();
  MoveGenerator::initAttackTables();

  UCI::run();

  return 0;
}
