#include <catch2/catch_test_macros.hpp>

#include "board.hpp"
#include "move-generator.hpp"
#include "perft.hpp"

/*
 * Perft tests are taken from here
 * https://www.chessprogramming.org/Perft_Results
 */

TEST_CASE("Initial position") {
  MoveGenerator::initAttackTables();

  Board board;
  CHECK(Perft::search(board, 5) == 4'865'609);
}

TEST_CASE("Position 2") {
  MoveGenerator::initAttackTables();

  Board board{
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"};
  CHECK(Perft::search(board, 4) == 4'085'603);
}

TEST_CASE("Position 3") {
  MoveGenerator::initAttackTables();

  Board board{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"};
  CHECK(Perft::search(board, 5) == 674'624);
}
TEST_CASE("Position 4") {
  MoveGenerator::initAttackTables();

  Board board{
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"};
  CHECK(Perft::search(board, 4) == 422'333);
}

TEST_CASE("Position 5") {
  MoveGenerator::initAttackTables();

  Board board{"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};
  CHECK(Perft::search(board, 5) == 89'941'194);
}

TEST_CASE("Position 6") {
  MoveGenerator::initAttackTables();

  Board board{"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w "
              "- - 0 10"};
  CHECK(Perft::search(board, 4) == 3'894'594);
}
