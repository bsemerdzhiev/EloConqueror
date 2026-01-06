#include "board.hpp"
#include "move.hpp"
#include "search.hpp"

#include <vector>

Board::Board() {
  // initialize kings
  _pieces[0][0] = getPositionAsBitboard(0, 4);
  _pieces[1][0] = getPositionAsBitboard(7, 4);

  // initialize queens
  _pieces[0][1] = getPositionAsBitboard(0, 3);
  _pieces[1][1] = getPositionAsBitboard(7, 3);

  // initialize rooks
  _pieces[0][2] = getPositionAsBitboard(0, 0);
  _pieces[0][2] |= getPositionAsBitboard(0, 7);
  _pieces[1][2] = getPositionAsBitboard(7, 0);
  _pieces[1][2] |= getPositionAsBitboard(7, 7);

  // initialize bishops
  _pieces[0][3] = getPositionAsBitboard(0, 2);
  _pieces[0][3] |= getPositionAsBitboard(0, 5);
  _pieces[1][3] = getPositionAsBitboard(7, 2);
  _pieces[1][3] |= getPositionAsBitboard(7, 5);

  // initialize knights
  _pieces[0][4] = getPositionAsBitboard(0, 1);
  _pieces[0][4] |= getPositionAsBitboard(0, 6);
  _pieces[1][4] = getPositionAsBitboard(7, 1);
  _pieces[1][4] |= getPositionAsBitboard(7, 6);

  // initialize the pawns
  _pieces[0][5] = ((1LL << BOARD_COLS) - 1) << BOARD_COLS;
  _pieces[1][5] = ((1LL << BOARD_COLS) - 1) << (BOARD_COLS * (BOARD_ROWS - 2));
}

Board Board::makeMove(int64_t from_pos, int64_t to_pos, int8_t piece_type,
                      bool turn) const {
  Board new_board = *this;
  new_board._player_turn ^= 1; // change player's turn

  new_board._pieces[turn][piece_type] ^= from_pos;
  new_board._pieces[turn][piece_type] ^= to_pos;

  for (std::size_t i{0}; i < ALL_PIECE_TYPES; i++) {
    new_board._pieces[turn ^ 1][i] &= ~to_pos; // clear the to_pos position
  }

  return new_board;
}

bool Board::isCellNotEmpty(int8_t piece_type, int64_t to_pos, bool turn) const {
  bool result = 0;
  for (std::size_t i{0}; i < Board::ALL_PIECE_TYPES; i++) {
    if (i != piece_type) {
      result |= _pieces[turn][i] & to_pos; // our figure
    }
  }

  return result;
}

bool Board::isUnderCheck(bool turn) const {

  std::vector<Move> attacked_squares =
      MoveExplorer::searchAllMoves(*this, turn ^ 1, false);

  bool result = false;
  for (const auto &move_to_check : attacked_squares) {
    result |= (move_to_check.pos_to & _pieces[turn ^ 1][0]);
  }
  return result;
}

inline int64_t Board::getPositionAsBitboard(int8_t row, int8_t col) {
  return (1LL << (row * BOARD_COLS + col));
}

int64_t Board::getPiece(int8_t piece_type, bool colour) const {
  return _pieces[piece_type][colour];
}
