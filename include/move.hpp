#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

struct Move {
  int64_t pos_from;
  int64_t pos_to;
  Move(int64_t pos_from_, int64_t pos_to_)
      : pos_from(pos_from_), pos_to(pos_to_) {}
};

#endif // !MOVE_H
