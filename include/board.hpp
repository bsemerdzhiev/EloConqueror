#ifndef BOARD_H
#define BOARD_H

#include "util.hpp"

#include <cstdint>
#include <string>

class Board {
public:
  static constexpr int8_t BOARD_ROWS = 8;
  static constexpr int8_t BOARD_COLS = 8;
  static constexpr int8_t ALL_PIECE_TYPES = 6;

  Board();

  Board(std::string fen_string);

  static inline uint64_t getPositionAsBitboard(int8_t row, int8_t col) {
    return (uint64_t{1} << (row * BOARD_COLS + col));
  }
  static inline uint64_t shiftPosition(uint64_t pos, int8_t dir,
                                       uint64_t mask) {
    if (dir < 0) {
      return (pos & (~mask)) >> (-dir);
    } else {
      return (pos & (~mask)) << dir;
    }
  }

  void displayBoard() const;

  static uint64_t chessSquareAsPosition(std::string chess_square);
  static std::string positionAsChessSquare(uint64_t pos);

  Board makeMove(uint64_t from_pos, uint64_t to_pos, int8_t piece_type,
                 bool turn, MoveType move_type) const;

  Board makeMove(const std::string &move_to_make) const;

  inline bool isCellNotEmpty(uint64_t to_pos, bool turn) const {
    int64_t res = 0;
    for (int32_t i = 0; i < ALL_PIECE_TYPES; i++) {
      res |= _pieces[turn][i];
    }

    return static_cast<bool>(res & to_pos);
  }

  bool isUnderCheck(uint64_t pos_to_check, bool turn) const;
  bool isEnPassant(uint64_t pos, bool turn) const;
  // 1 - short castle ... 0 - long castle
  bool checkCastlingRights(bool turn, bool castle_type) const;

  uint64_t getPiece(int8_t piece_type, bool turn) const;
  bool getPlayerTurn() const;
  uint64_t getLastMoveTwoSquaresPushPawn() const;

private:
  /*
   * elements at ind 0 represent white figures, 1 is for black
   * 0 - king
   * 1 - queen
   * 2 - rook
   * 3 - bishop
   * 4 - knight
   * 5 - pawn
   */
  uint64_t _pieces[2][6];
  // uint64_t _all_pieces[2];
  /*
   * Set to 0 if last move
   * was not a two square push from a pawn.
   * Set to the pawn's square otherwise
   */
  uint64_t _last_move_two_squares_push_pawn;
  uint64_t _pieces_not_moved;
  bool _player_turn;
};

#endif // !BOARD_H
