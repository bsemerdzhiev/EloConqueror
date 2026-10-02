#pragma once

#include "move.hpp"
#include "zobrist-hash.hpp"

#include <bit>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

inline const char *book_path = "../book-bins/book.bin";
inline constexpr std::size_t START_OFFSET = 0x60;

struct PolyglotEntry {
  uint64_t key;
  uint16_t move;
  uint16_t weight;
  uint32_t learn;

  void byteswap() {
    key = std::byteswap(key);
    move = std::byteswap(move);
    weight = std::byteswap(weight);
    learn = std::byteswap(learn);
  }
};

class Polyglot {
public:
  Polyglot();

  Polyglot(const Polyglot &) = delete;
  Polyglot &operator=(const Polyglot &) = delete;

  Polyglot(Polyglot &&) = delete;
  Polyglot &operator=(Polyglot &&) = delete;
  ~Polyglot();

  Move look_up(const Board &board);

private:
  int32_t fd_;
  void *mapping_;

  std::size_t file_length_;
  std::size_t number_of_entries_;

  PolyglotEntry *book_ptr_;
};

static_assert(sizeof(PolyglotEntry) == 16);

inline Polyglot polyglot;
