#include "zobrist-hash.hpp"

std::mt19937_64 ZobristHash::rng;

std::array<std::array<std::array<ZobristHash::ZobristHashValue,
                                 Board::ALL_PIECE_TYPES>,
                      Board::BOARD_COLS * Board::BOARD_ROWS>,
           2>
    ZobristHash::pieces_hash;

ZobristHash::ZobristHashValue ZobristHash::side_to_move_hash;

std::array<ZobristHash::ZobristHashValue, 4> ZobristHash::castling_hash;
std::array<ZobristHash::ZobristHashValue, 8> ZobristHash::en_passant_hash;

void ZobristHash::generateRandomNumbers() {
  for (std::size_t i{0}; i < Board::BOARD_COLS * Board::BOARD_ROWS; i++) {
    for (std::size_t j{0}; j < Board::ALL_PIECE_TYPES; j++) {
      pieces_hash[0][i][j] = rng();
      pieces_hash[1][i][j] = rng();
    }
  }

  side_to_move_hash = rng();

  for (std::size_t i{0}; i < 4; i++) {
    castling_hash[i] = rng();
  }

  for (std::size_t i{0}; i < 8; i++) {
    en_passant_hash[i] = rng();
  }
}
