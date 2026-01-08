#ifndef MOVE_H
#define MOVE_H

#include "board.hpp"
#include "util.hpp"

#include <cstdint>
#include <string>

struct Move {
  int64_t pos_from;
  int64_t pos_to;
  Pieces piece_type;
  MoveType move_type;

  Move(int64_t pos_from_, int64_t pos_to_, Pieces piece_type_,
       MoveType move_type_)
      : pos_from(pos_from_), pos_to(pos_to_), piece_type(piece_type_),
        move_type(move_type_) {}

  std::string formatted() const {

    std::string from_str = Board::positionAsChessSquare(pos_from);
    std::string to_str = Board::positionAsChessSquare(pos_to);

    std::string addition = "";

    switch (move_type) {
    case MoveType::PAWN_PROMOTE_QUEEN:
      addition = "q";
      break;
    case MoveType::PAWN_PROMOTE_ROOK:
      addition = "r";
      break;
    case MoveType::PAWN_PROMOTE_BISHOP:
      addition = "b";
      break;
    case MoveType::PAWN_PROMOTE_KNIGHT:
      addition = "n";
      break;
    default:
      break;
    }

    return from_str + to_str + addition;
  }
};

#endif // !MOVE_H
