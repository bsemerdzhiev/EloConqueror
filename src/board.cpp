#include "board.hpp"
#include "search.hpp"
#include "util.hpp"

#include <bit>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
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
  _pieces[0][5] = ((uint64_t{1} << BOARD_COLS) - int64_t{1}) << BOARD_COLS;
  _pieces[1][5] = ((uint64_t{1} << BOARD_COLS) - int64_t{1})
                  << (BOARD_COLS * (BOARD_ROWS - 2));

  // used for checking castling rights
  _pieces_not_moved =
      _pieces[0][2] | _pieces[1][2] | _pieces[0][0] | _pieces[1][0];
}

Board::Board(std::string fen_string) {
  for (std::size_t colour{0}; colour < 2; colour++) {
    for (std::size_t piece_type{0}; piece_type < Board::ALL_PIECE_TYPES;
         piece_type++) {
      _pieces[colour][piece_type] = 0;
    }
  }

  size_t whitespace_loc = fen_string.find(" ");

  int32_t cur_row = 7;
  int32_t piece_col = 0;

  for (const char letter_to_parse : fen_string.substr(0, whitespace_loc)) {
    if (letter_to_parse >= '1' && letter_to_parse <= '8') {
      piece_col += letter_to_parse - '0';
    } else if (letter_to_parse == '/') {
      piece_col = 0;
      cur_row--;
    } else {
      bool player_turn = (letter_to_parse >= 'a' && letter_to_parse <= 'z');
      int32_t piece_type = 0;

      switch (std::tolower(letter_to_parse)) {
      case 'k':
        piece_type = 0;
        break;
      case 'q':
        piece_type = 1;
        break;
      case 'r':
        piece_type = 2;
        break;
      case 'b':
        piece_type = 3;
        break;
      case 'n':
        piece_type = 4;
        break;
      case 'p':
        piece_type = 5;
        break;
      }
      // std::cout << cur_row << " " << piece_col << " " << piece_type
      //           << std::endl;
      //  std::cout << cur_row << std::endl;
      _pieces[player_turn][piece_type] |=
          Board::getPositionAsBitboard(cur_row, piece_col);
      piece_col += 1;
    }
  }

  // who's turn
  whitespace_loc = fen_string.find(" ", whitespace_loc + 1);

  _player_turn = fen_string[whitespace_loc - 1] == 'b';

  // handles castling
  _pieces_not_moved = 0;

  std::size_t castling_rights_start = whitespace_loc + 1;
  whitespace_loc = fen_string.find(" ", castling_rights_start);

  for (const char letter_to_parse : fen_string.substr(
           castling_rights_start, whitespace_loc - castling_rights_start)) {
    switch (letter_to_parse) {
    case 'K':
      _pieces_not_moved |= Board::getPositionAsBitboard(0, 4);
      _pieces_not_moved |= Board::getPositionAsBitboard(0, 7);
      break;
    case 'k':
      _pieces_not_moved |= Board::getPositionAsBitboard(7, 4);
      _pieces_not_moved |= Board::getPositionAsBitboard(7, 7);
      break;
    case 'Q':
      _pieces_not_moved |= Board::getPositionAsBitboard(0, 4);
      _pieces_not_moved |= Board::getPositionAsBitboard(0, 0);
      break;
    case 'q':
      _pieces_not_moved |= Board::getPositionAsBitboard(7, 4);
      _pieces_not_moved |= Board::getPositionAsBitboard(7, 0);
      break;
    }
  }

  std::size_t enpassant_attack_square_start = whitespace_loc + 1;
  whitespace_loc = fen_string.find(" ", enpassant_attack_square_start);

  _last_move_two_squares_push_pawn = chessSquareAsPosition(
      fen_string.substr(enpassant_attack_square_start,
                        whitespace_loc - enpassant_attack_square_start));

  // TODO halfmove counter

  // TODO fullmove counter
}

Board Board::makeMove(const std::string &move_to_make) const {

  std::vector<Move> all_moves;
  all_moves.resize(256);
  MoveExplorer::searchAllMoves(*this, _player_turn, all_moves);

  for (const auto &possible_move : all_moves) {
    if (possible_move.formatted() == move_to_make) {
      return makeMove(possible_move.pos_from, possible_move.pos_to,
                      possible_move.piece_type, _player_turn,
                      possible_move.move_type);
    }
  }

  return Board();
}

uint64_t Board::chessSquareAsPosition(std::string chess_square) {
  if (chess_square == "-") {
    return 0;
  }

  int8_t col = chess_square[0] - 'a';
  int8_t row = chess_square[1] - '1';

  return getPositionAsBitboard(row, col);
}

bool Board::checkCastlingRights(bool turn, bool castle_type) const {
  int8_t row_to_use = turn ? 7 : 0;
  int8_t col_to_use = castle_type ? 7 : 0;

  uint64_t wanted_positions =
      Board::getPositionAsBitboard(row_to_use, 4) |
      Board::getPositionAsBitboard(row_to_use, col_to_use);

  return (wanted_positions & _pieces_not_moved) == wanted_positions;
}

bool Board::isEnPassant(uint64_t pos, bool turn) const {
  return _last_move_two_squares_push_pawn == pos;
}

Board Board::makeMove(uint64_t from_pos, uint64_t to_pos, int8_t piece_type,
                      bool turn, MoveType move_type) const {
  Board new_board = *this;

  new_board._player_turn = turn ^ 1; // change player's turn

  new_board._pieces_not_moved &=
      ~(from_pos | to_pos); // mark the current cell as moved

  new_board._pieces[turn][piece_type] ^= from_pos;

  new_board._last_move_two_squares_push_pawn = 0;

  // new_board._all_pieces[turn] ^= from_pos;
  // new_board._all_pieces[turn] ^= to_pos;

  switch (move_type) {
  case MoveType::PAWN_PROMOTE_QUEEN:
    new_board._pieces[turn][Pieces::QUEEN] ^= to_pos;
    break;
  case MoveType::PAWN_PROMOTE_ROOK:
    new_board._pieces[turn][Pieces::ROOK] ^= to_pos;
    break;
  case MoveType::PAWN_PROMOTE_BISHOP:
    new_board._pieces[turn][Pieces::BISHOP] ^= to_pos;
    break;
  case MoveType::PAWN_PROMOTE_KNIGHT:
    new_board._pieces[turn][Pieces::KNIGHT] ^= to_pos;
    break;
  case MoveType::PAWN_MOVE_TWO_SQUARES: {
    if (turn) {
      new_board._last_move_two_squares_push_pawn = (to_pos << 8);
    } else {
      new_board._last_move_two_squares_push_pawn = (to_pos >> 8);
    }

    new_board._pieces[turn][piece_type] ^= to_pos;
    break;
  }
  case MoveType::SHORT_CASTLE_KING_MOVE: {
    new_board._pieces[turn][piece_type] ^= to_pos;

    new_board._pieces[turn][Pieces::ROOK] ^= MoveExplorer::rook_from[turn][1];
    new_board._pieces[turn][Pieces::ROOK] ^= MoveExplorer::rook_to[turn][1];

    // new_board._all_pieces[turn] ^= MoveExplorer::rook_from[turn][1];
    // new_board._all_pieces[turn] ^= MoveExplorer::rook_from[turn][1];
    return new_board;
  }
  case MoveType::LONG_CASTLE_KING_MOVE: {
    new_board._pieces[turn][piece_type] ^= to_pos;

    new_board._pieces[turn][Pieces::ROOK] ^= MoveExplorer::rook_from[turn][0];
    new_board._pieces[turn][Pieces::ROOK] ^= MoveExplorer::rook_to[turn][0];

    // new_board._all_pieces[turn] ^= MoveExplorer::rook_from[turn][0];
    // new_board._all_pieces[turn] ^= MoveExplorer::rook_from[turn][0];

    return new_board;
  }
  default:
    new_board._pieces[turn][piece_type] ^= to_pos;
    break;
  }

  for (std::size_t i{0}; i < ALL_PIECE_TYPES; i++) {
    /* clear the to_pos position
     * where the pawn
     * that moved two squares actually is
     */
    if (_last_move_two_squares_push_pawn == to_pos &&
        move_type == MoveType::REGULAR_PAWN_CAPTURE) {
      if (turn) {
        new_board._pieces[turn ^ 1][i] &= ~(to_pos << 8);
        // new_board._all_pieces[turn ^ 1] &= ~(to_pos << 8);
      } else {
        new_board._pieces[turn ^ 1][i] &= ~(to_pos >> 8);
        // new_board._all_pieces[turn ^ 1] &= ~(to_pos >> 8);
      }
    }

    new_board._pieces[turn ^ 1][i] &= ~to_pos; // clear the to_pos position
    // new_board._all_pieces[turn ^ 1] &= ~to_pos;
  }

  return new_board;
}

bool Board::isUnderCheck(const uint64_t pos_to_check, bool turn) const {

  const uint64_t king_pos = pos_to_check;

  // check for line checks
  for (std::size_t i{0}; i < MoveExplorer::combined_shifts.size(); i++) {
    uint64_t cell_under_investigation = king_pos;
    int8_t shift_dir = MoveExplorer::combined_shifts[i];
    uint64_t mask = MoveExplorer::combined_shifts_masks[i];

    cell_under_investigation =
        shiftPosition(cell_under_investigation, shift_dir, mask);

    int8_t steps = 1;
    while (cell_under_investigation) {
      if (isCellNotEmpty(cell_under_investigation, turn)) {
        break;
      } else if (isCellNotEmpty(cell_under_investigation, turn ^ 1)) {
        // check if it's a bishop
        if (((_pieces[turn ^ 1][Pieces::BISHOP] & cell_under_investigation) &&
             (i < 4)) ||
            ((_pieces[turn ^ 1][Pieces::ROOK] & cell_under_investigation) &&
             (i >= 4)) ||
            ((_pieces[turn ^ 1][Pieces::QUEEN] & cell_under_investigation)) ||
            ((_pieces[turn ^ 1][Pieces::KING] & cell_under_investigation) &&
             (steps == 1))) {
          return true;
        } else {
          break;
        }
      }
      steps++;
      cell_under_investigation =
          shiftPosition(cell_under_investigation, shift_dir, mask);
    }
  }

  // check for pawn checks
  uint64_t pawn_positions =
      Board::shiftPosition(
          king_pos, turn ? -9 : +7,
          MoveExplorer::FILE_A |
              (turn ? MoveExplorer::ROW_ONE : MoveExplorer::ROW_SEVEN)) |
      Board::shiftPosition(
          king_pos, turn ? -7 : +9,
          MoveExplorer::FILE_H |
              (turn ? MoveExplorer::ROW_ONE : MoveExplorer::ROW_SEVEN));
  if (_pieces[turn ^ 1][Pieces::PAWN] & pawn_positions) {
    return true;
  }

  int64_t knight_positions = 0;
  for (std::size_t i{0}; i < MoveExplorer::knight_move_shifts.size(); i++) {
    const uint64_t to_check_for_knight_pos =
        Board::shiftPosition(king_pos, MoveExplorer::knight_move_shifts[i],
                             MoveExplorer::knight_move_shifts_masks[i]);

    knight_positions |= to_check_for_knight_pos;
  }
  if (_pieces[turn ^ 1][Pieces::KNIGHT] & knight_positions) {
    return true;
  }

  return false;
}

void Board::displayBoard() const {
  std::cout << (_player_turn ? "Black to turn" : "White to turn") << "\n";

  std::cout << checkCastlingRights(0, 1) << "\n";
  std::cout << checkCastlingRights(0, 0) << "\n";
  std::cout << checkCastlingRights(1, 1) << "\n";
  std::cout << checkCastlingRights(1, 0) << "\n";

  std::cout << positionAsChessSquare(_last_move_two_squares_push_pawn) << "\n";

  const std::array<char, 7> piece_type_to_char = {'k', 'q', 'r', 'b',
                                                  'n', 'p', ' '};

  for (std::size_t j{0}; j < BOARD_COLS * 4; j++) {
    std::cout << "-";
  }

  std::cout << "\n";
  for (std::int32_t i = BOARD_ROWS - 1; i >= 0; i--) {

    std::cout << "| ";
    for (std::int32_t j = 0; j < BOARD_COLS; j++) {
      std::int8_t piece_type = 6;
      std::int8_t piece_colour = 1;

      for (std::size_t colour{0}; colour < 2; colour++) {
        for (std::size_t piece_to_check{0}; piece_to_check < ALL_PIECE_TYPES;
             piece_to_check++) {
          if (_pieces[colour][piece_to_check] &
              Board::getPositionAsBitboard(i, j)) {
            piece_colour = colour;
            piece_type = piece_to_check;
          }
        }
      }

      if (piece_colour == 0) {
        std::cout << char(toupper(piece_type_to_char[piece_type]));
      } else {
        std::cout << piece_type_to_char[piece_type];
      }
      std::cout << " | ";
    }
    std::cout << "\n";
    for (std::size_t j{0}; j < BOARD_COLS * 4; j++) {
      std::cout << "-";
    }

    std::cout << "\n";
  }
}

std::string Board::positionAsChessSquare(uint64_t pos) {
  if (pos == 0) {
    return "-";
  }

  std::size_t arr_pos = std::__countr_zero(pos);

  int8_t row = arr_pos / BOARD_COLS;
  int8_t cols = arr_pos % BOARD_COLS;

  return std::string{char(cols + 'a'), char(row + '1')};
}

uint64_t Board::getPiece(int8_t piece_type, bool colour) const {
  return _pieces[colour][piece_type];
}

bool Board::getPlayerTurn() const { return _player_turn; }
