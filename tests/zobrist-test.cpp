#include "zobrist-hash.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Zobrist test 1") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"});

  CHECK(val == 0x463b96181691fc9c);
}

TEST_CASE("Zobrist test 2") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"});

  CHECK(val == 0x823c9b50fd114196);
}

TEST_CASE("Zobrist test 3") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2"});

  CHECK(val == 0x0756b94461c50fb0);
}

TEST_CASE("Zobrist test 4") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2"});

  CHECK(val == 0x662fafb965db29d4);
}

TEST_CASE("Zobrist test 5") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"});

  CHECK(val == 0x22a48b5a8e47ff78);
}

TEST_CASE("Zobrist test 6") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3"});

  CHECK(val == 0x652a607ca3f242c1);
}

TEST_CASE("Zobrist test 7") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4"});

  CHECK(val == 0x00fdd303c946bdd9);
}

TEST_CASE("Zobrist test 8") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3 0 3"});

  CHECK(val == 0x3c8123ea7b067637);
}

TEST_CASE("Zobrist test 9") {
  ZobristHash::ZobristHashValue val = ZobristHash::hashBoard(
      Board{"rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq - 0 4"});

  CHECK(val == 0x5c3f9b829b279560);
}
