#ifndef BOARD_INHL_H
#define BOARD_INHL_H

#include "board.hpp"
#include "move-generator.hpp"
#include "util.hpp"
#include <cstdint>

namespace BoardInl {
[[__gnu__::__always_inline__]]
inline bool cellIsUnderAttack(const Board &board, const uint64_t pos_to_check,
                              const bool turn) {

  if (pos_to_check == 0) { // is mated
    return true;
  }

  const uint32_t king_sq = std::__countr_zero(pos_to_check);

  uint64_t cell_under_investigation;

  const uint64_t enemy_rq = board._pieces[turn ^ 1][Pieces::ROOK] |
                            board._pieces[turn ^ 1][Pieces::QUEEN];
  const uint64_t enemy_bq = board._pieces[turn ^ 1][Pieces::BISHOP] |
                            board._pieces[turn ^ 1][Pieces::QUEEN];
  const uint64_t enemy_p = board._pieces[turn ^ 1][Pieces::PAWN];
  const uint64_t enemy_n = board._pieces[turn ^ 1][Pieces::KNIGHT];
  const uint64_t enemy_k = board._pieces[turn ^ 1][Pieces::KING];

  const uint64_t occ = board._all_pieces[0] | board._all_pieces[1];

  // check diagonals

  const auto msb = [](uint64_t nmb) {
    return nmb ? (1ULL << (63 - std::countl_zero(nmb))) : 0;
  };

  const auto lsb = [](uint64_t nmb) { return nmb & (-nmb); };

  uint64_t diag_matched = 0;

  diag_matched |= msb(MoveGenerator::DIAG_ATTACK_SQUARES[king_sq][0] & occ);
  diag_matched |= msb(MoveGenerator::DIAG_ATTACK_SQUARES[king_sq][1] & occ);

  diag_matched |= lsb(MoveGenerator::DIAG_ATTACK_SQUARES[king_sq][2] & occ);
  diag_matched |= lsb(MoveGenerator::DIAG_ATTACK_SQUARES[king_sq][3] & occ);

  uint64_t line_matched = 0;

  line_matched |= msb(MoveGenerator::LINE_ATTACK_SQUARES[king_sq][0] & occ);

  line_matched |= lsb(MoveGenerator::LINE_ATTACK_SQUARES[king_sq][1] & occ);
  line_matched |= msb(MoveGenerator::LINE_ATTACK_SQUARES[king_sq][2] & occ);
  line_matched |= lsb(MoveGenerator::LINE_ATTACK_SQUARES[king_sq][3] & occ);

  if ((diag_matched & enemy_bq) || (line_matched & enemy_rq)) {
    return true;
  }

  if (enemy_k & MoveGenerator::KING_ATTACK_SQUARES[king_sq]) {
    return true;
  }

  if (enemy_p & MoveGenerator::PAWN_ATTACK_SQUARES[king_sq][turn]) {
    return true;
  }

  if (enemy_n & MoveGenerator::KNIGHT_ATTACK_SQUARES[king_sq]) {
    return true;
  }

  return false;
}
} // namespace BoardInl

#endif
