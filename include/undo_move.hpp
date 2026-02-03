#ifndef UNDO_MOVE_H
#define UNDO_MOVE_H

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

#endif // UNDO_MOVE_H
