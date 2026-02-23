#include "board.hpp"
#include "evaluate.hpp"
#include <cassert>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Initial position evaluation") {
  Board board{};

  Evaluate::initTables();
  int32_t evaluation = Evaluate::evaluateBoard(board);

  CHECK(evaluation == 0);
}

TEST_CASE("Classical equal mid-game") {
  Board board{
      "r2q1rk1/ppp2ppp/2np1n2/4p3/2B1P3/2NP1N2/PPP2PPP/R1BQ1RK1 w - - 0 8"};

  Evaluate::initTables();
  int32_t evaluation = Evaluate::evaluateBoard(board);

  CHECK((evaluation > 600 && evaluation < 750));
}

TEST_CASE("Slight space advantage") {
  Board board{
      "rnbq1rk1/ppp2ppp/3p1n2/4p3/2BPP3/5N2/PPP2PPP/RNBQ1RK1 w - - 0 7"};

  Evaluate::initTables();
  int32_t evaluation = Evaluate::evaluateBoard(board);

  CHECK((evaluation > 200 && evaluation < 500));
}
