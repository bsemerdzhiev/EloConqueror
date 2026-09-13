#pragma once

#include "board.hpp"
#include "util.hpp"

#include <algorithm>
#include <cstdint>
#include <string>

enum class MovePriority : uint8_t {
  HashedMove = 0,
  Capture = 1,
  UnknownMove = 2,
};

struct Move {
  uint64_t pos_from;
  uint64_t pos_to;
  MoveType move_type;
  Pieces piece_type;
  bool captures;

  Move() = default;
  Move(uint64_t pos_from_, uint64_t pos_to_, Pieces piece_type_,
       MoveType move_type_, bool captures_)
      : pos_from(pos_from_), pos_to(pos_to_), piece_type(piece_type_),
        move_type(move_type_), captures(captures_) {}

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

  bool operator==(const Move other) const {
    return pos_from == other.pos_from && pos_to == other.pos_to &&
           move_type == other.move_type;
  }

  MovePriority evaluatePriority(const Move *pv) {
    if (pv != nullptr && *pv == *this) {
      return MovePriority::HashedMove;
    }

    // check if its a capture move
    if (captures) {
      return MovePriority::Capture;
    }

    return MovePriority::UnknownMove;
  }
};

template <typename Container>
typename Container::iterator
getBestMove(Container &container, typename Container::iterator cur_iterator,
            const Move *pv) {
  const auto initial_it = cur_iterator;
  auto end_it = container.end();

  typename Container::iterator best_move = cur_iterator;
  MovePriority highest_priority = MovePriority::UnknownMove;

  while (cur_iterator != end_it) {
    auto cur_priority = cur_iterator->evaluatePriority(pv);

    if (cur_priority < highest_priority) {
      highest_priority = cur_priority;
      best_move = cur_iterator;
    }

    cur_iterator = next(cur_iterator);
  }

  std::iter_swap(best_move, initial_it);

  return initial_it;
}
