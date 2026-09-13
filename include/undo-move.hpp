#pragma once

#include "util.hpp"
#include <cstdint>

struct UndoMove {
  uint64_t prev_enpassant_pos;
  uint64_t from_pos;
  uint64_t to_pos;
  uint64_t pieces_not_moved;

  int8_t piece_type;

  int8_t taken_piece;
  MoveType move_type;
};
