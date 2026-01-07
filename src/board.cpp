#include "board.hpp"
#include "move.hpp"
#include "search.hpp"

#include <vector>

Board::Board() {
  _last_move_two_squares_push_pawn = 0;
  _player_turn = false; // white starts first

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
  _pieces[0][5] = ((int64_t{1} << BOARD_COLS) - int64_t{1}) << BOARD_COLS;
  _pieces[1][5] = ((int64_t{1} << BOARD_COLS) - int64_t{1})
                  << (BOARD_COLS * (BOARD_ROWS - 2));

  // used for checking castling rights
  _pieces_not_moved =
      _pieces[0][2] | _pieces[1][2] | _pieces[0][0] | _pieces[1][0];
}

bool Board::checkCastlingRights(bool turn, bool castle_type) const {
  int8_t row_to_use = turn ? 7 : 0;
  int8_t col_to_use = castle_type ? 7 : 0;

  int64_t wanted_positions =
      Board::getPositionAsBitboard(row_to_use, 4) |
      Board::getPositionAsBitboard(row_to_use, col_to_use);

  return (wanted_positions & _pieces_not_moved) == wanted_positions;
}

int64_t Board::getLastMoveTwoSquaresPushPawn() const {
  return _last_move_two_squares_push_pawn;
}

bool Board::isEnPassant(int64_t pos, bool turn) const {
  return _last_move_two_squares_push_pawn == pos;
}

Board Board::makeMove(int64_t from_pos, int64_t to_pos, int8_t piece_type,
                      bool turn, MoveType move_type) const {
  Board new_board = *this;
  new_board._player_turn ^= 1; // change player's turn

  new_board._pieces_not_moved &= ~from_pos; // mark the current cell as moved

  new_board._pieces[turn][piece_type] ^= from_pos;
  new_board._pieces[turn][piece_type] ^= to_pos;

  new_board._last_move_two_squares_push_pawn = 0;
  if (move_type == MoveType::PAWN_MOVE_TWO_SQUARES) {
    new_board._last_move_two_squares_push_pawn = to_pos;
  } else if (move_type == MoveType::SHORT_CASTLE_KING_MOVE) {
    int8_t row_to_use = turn ? 7 : 0;

    new_board._pieces[turn][2] ^= Board::getPositionAsBitboard(row_to_use, 7);
    new_board._pieces[turn][2] ^= Board::getPositionAsBitboard(row_to_use, 5);

    return new_board;
  } else if (move_type == MoveType::LONG_CASTLE_KING_MOVE) {
    int8_t row_to_use = turn ? 7 : 0;

    new_board._pieces[turn][2] ^= Board::getPositionAsBitboard(row_to_use, 0);
    new_board._pieces[turn][2] ^= Board::getPositionAsBitboard(row_to_use, 3);
    return new_board;
  }

  for (std::size_t i{0}; i < ALL_PIECE_TYPES; i++) {

    /* clear the to_pos position
     * where the pawn
     * that moved two squares actually is
     */
    if (_last_move_two_squares_push_pawn == to_pos &&
        move_type == MoveType::REGULAR_PAWN_CAPTURE) {
      int8_t direction = turn ? -8 : +8;
      if (turn) {
        new_board._pieces[turn ^ 1][i] &= ~(to_pos << 8);
      } else {
        new_board._pieces[turn ^ 1][i] &= ~(to_pos >> 8);
      }
    }

    new_board._pieces[turn ^ 1][i] &= ~to_pos; // clear the to_pos position
  }

  return new_board;
}

bool Board::isCellNotEmpty(int64_t to_pos, bool turn) const {
  bool result = 0;
  for (std::size_t i{0}; i < Board::ALL_PIECE_TYPES; i++) {
    result |= _pieces[turn][i] & to_pos;
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

int64_t Board::getPiece(int8_t piece_type, bool colour) const {
  return _pieces[colour][piece_type];
}

bool Board::getPlayerTurn() const { return _player_turn; }
