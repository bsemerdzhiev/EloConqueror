#ifndef BOARD_H
#define BOARD_H

#include <cstdint>

class Board {
public:
  static constexpr int8_t BOARD_ROWS = 8;
  static constexpr int8_t BOARD_COLS = 8;
  static constexpr int8_t ALL_PIECE_TYPES = 6;

  Board();

  static inline int64_t getPositionAsBitboard(int8_t row, int8_t col) {
    return (int64_t{1} << (row * BOARD_COLS + col));
  }

  Board makeMove(int64_t from_pos, int64_t to_pos, int8_t piece_type,
                 bool turn) const;

  bool isCellNotEmpty(int64_t to_pos, bool turn) const;
  bool isUnderCheck(bool turn) const;

  int64_t getPiece(int8_t piece_type, bool turn) const;
  bool getPlayerTurn() const;

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
  int64_t _pieces[2][6];
  bool _player_turn;
};

#endif // !BOARD_H
