#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

enum class MoveType {
  REGULAR_KING_MOVE,
  LONG_CASTLE_KING_MOVE,
  SHORT_CASTLE_KING_MOVE,
  QUEEN_MOVE,
  ROOK_MOVE,
  BISHOP_MOVE,
  KNIGHT_MOVE,
  REGULAR_PAWN_CAPTURE,
  EN_PASSANT_PAWN_CAPTURE,
  PAWN_MOVE,
  PAWN_MOVE_TWO_SQUARES
};

struct Move {
  int64_t pos_from;
  int64_t pos_to;
  MoveType move_type;

  Move(int64_t pos_from_, int64_t pos_to_, MoveType move_type_)
      : pos_from(pos_from_), pos_to(pos_to_), move_type(move_type_) {}
};

#endif // !MOVE_H
