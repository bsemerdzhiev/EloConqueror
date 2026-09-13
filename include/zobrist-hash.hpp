#pragma once

#include "board.hpp"
#include <array>
#include <bit>
#include <random>
namespace ZobristHash {
using ZobristHashValue = uint64_t;

extern std::mt19937_64 rng;

extern std::array<
    std::array<std::array<ZobristHashValue, Board::ALL_PIECE_TYPES>,
               Board::BOARD_COLS * Board::BOARD_ROWS>,
    2>
    pieces_hash;
extern ZobristHashValue side_to_move_hash;

extern std::array<ZobristHashValue, 4> castling_hash;
extern std::array<ZobristHashValue, 8> en_passant_hash;

void generateRandomNumbers();

inline ZobristHashValue hashBoard(const Board &board) {
  ZobristHashValue result{};

  // hash the positions of all the pieces
  for (std::size_t color{0}; color < 2; color++) {
    for (std::size_t i{0}; i < Board::ALL_PIECE_TYPES; i++) {
      uint64_t pieces_positions = board._pieces[color][i];

      while (pieces_positions) {
        int32_t first_occupied_pos = std::countr_zero(pieces_positions);

        result ^= pieces_hash[color][first_occupied_pos][i];

        pieces_positions &= pieces_positions - 1;
      }
    }
  }

  // hash the player's turn
  if (board._player_turn) {
    result ^= side_to_move_hash;
  }

  // hash the castling rights
  for (std::size_t turn{0}; turn < 2; turn++) {
    for (std::size_t castle_type{0}; castle_type < 2; castle_type++) {
      if (board.checkCastlingRights(turn, castle_type)) {
        result ^= castling_hash[(turn << 1) | castle_type];
      }
    }
  }

  // hash the en-passant collumn
  if (board._last_move_two_squares_push_pawn) {
    int32_t col_for_en_passant =
        std::countr_zero(board._last_move_two_squares_push_pawn) % 8;

    result ^= en_passant_hash[col_for_en_passant];
  }

  return result;
}
}; // namespace ZobristHash
