#include "polyglot-book.hpp"
#include "board.hpp"
#include "util.hpp"
#include "zobrist-hash.hpp"
#include <algorithm>
#include <bit>
#include <cassert>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

Polyglot::Polyglot() {
  fd_ = open(book_path, O_RDONLY);

  if (fd_ == -1) {
    throw std::bad_exception();
  }

  struct stat file_stat;
  fstat(fd_, &file_stat);

  file_length_ = file_stat.st_size;

  mapping_ = mmap(nullptr, file_length_, PROT_READ, MAP_PRIVATE, fd_, 0);

  if ((file_length_ - START_OFFSET) % sizeof(PolyglotEntry) != 0) {
    throw std::runtime_error("Invalid Polyglot book");
  }
  number_of_entries_ = (file_length_ - START_OFFSET) / sizeof(PolyglotEntry);

  book_ptr_ = reinterpret_cast<PolyglotEntry *>(
      static_cast<std::byte *>(mapping_) + START_OFFSET);

  close(fd_);
}

Polyglot::~Polyglot() {
  if (fd_ != -1) {
    munmap(mapping_, file_length_);
  }
}

std::vector<Move> moves_buffer;

Move Polyglot::look_up(const Board &board) {
  ZobristHash::ZobristHashValue zobrist_key = ZobristHash::hashBoard(board);

  // std::cout << value << "\n";

  auto it = std::lower_bound(book_ptr_, book_ptr_ + number_of_entries_,
                             zobrist_key, [](const auto &entry, uint64_t key) {
                               return std::byteswap(entry.key) < key;
                             });

  if (it == book_ptr_ + number_of_entries_ ||
      std::byteswap(it->key) != zobrist_key) {
    return {};
  }

  PolyglotEntry entry = *it;
  entry.byteswap();

  Move chosen_move{};

  uint8_t to_file = entry.move & (0b111);
  uint8_t to_row = (entry.move >> 3) & (0b111);

  uint8_t from_file = (entry.move >> 6) & (0b111);
  uint8_t from_row = (entry.move >> 9) & (0b111);

  uint8_t promotion_piece = (entry.move >> 12) & (0b111);

  chosen_move.pos_from = (1ULL << ((from_row << 3) | from_file));
  chosen_move.pos_to = (1ULL << ((to_row << 3) | to_file));

  // check if the king is in pos_from and if we are attempting to castle

  uint64_t king_pos = board.getPiece(Pieces::KING, board._player_turn);

  switch (promotion_piece) {
  case 0: {
    // not important
    if (king_pos == chosen_move.pos_from &&
        ((to_file == 0 || to_file == 7) && from_file == 4)) {
      if (to_file == 0) {
        chosen_move.move_type = MoveType::LONG_CASTLE_KING_MOVE;
      } else {
        chosen_move.move_type = MoveType::SHORT_CASTLE_KING_MOVE;
      }
    } else {
      chosen_move.move_type = MoveType::REGULAR_KING_MOVE;
    }

    break;
  }
  case 1:
    chosen_move.move_type = MoveType::PAWN_PROMOTE_KNIGHT;
    break;
  case 2:
    chosen_move.move_type = MoveType::PAWN_PROMOTE_BISHOP;
    break;
  case 3:
    chosen_move.move_type = MoveType::PAWN_PROMOTE_ROOK;
    break;
  case 4:
    chosen_move.move_type = MoveType::PAWN_PROMOTE_QUEEN;
    break;
  }

  moves_buffer.clear();
  MoveGenerator::generatePseudoLegalMoves(board, moves_buffer);

  for (const auto &cur_move : moves_buffer) {
    if (cur_move.pos_from == chosen_move.pos_from &&
        cur_move.pos_to == chosen_move.pos_to) {
      if (chosen_move.move_type != MoveType::REGULAR_KING_MOVE) {
        if (chosen_move.move_type == cur_move.move_type) {
          chosen_move = cur_move;
        }
      } else {
        chosen_move = cur_move;
      }
    } else if (chosen_move.move_type == cur_move.move_type &&
               (chosen_move.move_type == MoveType::LONG_CASTLE_KING_MOVE ||
                chosen_move.move_type == MoveType::SHORT_CASTLE_KING_MOVE)) {
      chosen_move = cur_move;
    }
  }

  assert(chosen_move.pos_from != 0);

  return chosen_move;
}
