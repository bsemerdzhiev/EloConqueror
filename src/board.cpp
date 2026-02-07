#include "board.hpp"
#include "move.hpp"
#include "search.hpp"
#include "undo_move.hpp"
#include "util.hpp"

#include <bit>
#include <cctype>
#include <cstdint>
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

  recomputePiecesPositions();
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
  recomputePiecesPositions();
}

void Board::makeMove(const std::string &move_to_make) {

  std::vector<Move> all_moves;
  all_moves.resize(256);
  MoveExplorer::searchAllMoves(*this, _player_turn, all_moves);

  UndoMove undo_move;
  for (const auto &possible_move : all_moves) {
    if (possible_move.formatted() == move_to_make) {
      makeMove(possible_move, undo_move);
    }
  }
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

void Board::unmakeMove(const UndoMove &undo_move) {
  _player_turn ^= 1;

  _pieces_not_moved = undo_move.pieces_not_moved;
  _last_move_two_squares_push_pawn = undo_move.prev_enpassant_pos;

  _pieces[_player_turn][undo_move.piece_type] ^= undo_move.from_pos;

  switch (undo_move.move_type) {
  case MoveType::PAWN_PROMOTE_QUEEN: {
    _pieces[_player_turn][Pieces::QUEEN] ^= undo_move.to_pos;
    break;
  }
  case MoveType::PAWN_PROMOTE_ROOK: {
    _pieces[_player_turn][Pieces::ROOK] ^= undo_move.to_pos;
    break;
  }
  case MoveType::PAWN_PROMOTE_BISHOP: {
    _pieces[_player_turn][Pieces::BISHOP] ^= undo_move.to_pos;
    break;
  }
  case MoveType::PAWN_PROMOTE_KNIGHT: {
    _pieces[_player_turn][Pieces::KNIGHT] ^= undo_move.to_pos;
    break;
  }
  case MoveType::PAWN_MOVE_TWO_SQUARES: {
    _pieces[_player_turn][Pieces::PAWN] ^= undo_move.to_pos;
    break;
  }
  case MoveType::SHORT_CASTLE_KING_MOVE: {
    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_from[_player_turn][1];
    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_to[_player_turn][1];

    _pieces[_player_turn][Pieces::KING] ^= undo_move.to_pos;
    break;
  }
  case MoveType::LONG_CASTLE_KING_MOVE: {
    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_from[_player_turn][0];
    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_to[_player_turn][0];

    _pieces[_player_turn][Pieces::KING] ^= undo_move.to_pos;
    break;
  }
  default: {
    _pieces[_player_turn][undo_move.piece_type] ^= undo_move.to_pos;
  }
  }

  if (undo_move.taken_piece != -1) {
    if (undo_move.move_type == MoveType::REGULAR_PAWN_CAPTURE &&
        undo_move.to_pos == undo_move.prev_enpassant_pos) {
      _pieces[_player_turn ^ 1][undo_move.taken_piece] ^=
          Board::shiftPosition(undo_move.to_pos, _player_turn ? +8 : -8, 0);
    } else {
      _pieces[_player_turn ^ 1][undo_move.taken_piece] ^= undo_move.to_pos;
    }
  }
}

void Board::makeMove(const Move &move_to_make, UndoMove &undo_move) {
  undo_move.pieces_not_moved = _pieces_not_moved;
  _pieces_not_moved &= ~(move_to_make.pos_from |
                         move_to_make.pos_to); // mark the current cell as moved

  undo_move.from_pos = move_to_make.pos_from;
  undo_move.to_pos = move_to_make.pos_to;
  undo_move.taken_piece = -1;

  undo_move.piece_type = move_to_make.piece_type;
  _pieces[_player_turn][move_to_make.piece_type] ^= move_to_make.pos_from;

  undo_move.prev_enpassant_pos = _last_move_two_squares_push_pawn;
  _last_move_two_squares_push_pawn = 0;

  undo_move.move_type = move_to_make.move_type;
  switch (move_to_make.move_type) {
  case MoveType::PAWN_PROMOTE_QUEEN:
    _pieces[_player_turn][Pieces::QUEEN] ^= move_to_make.pos_to;
    break;
  case MoveType::PAWN_PROMOTE_ROOK:
    _pieces[_player_turn][Pieces::ROOK] ^= move_to_make.pos_to;
    ;
    break;
  case MoveType::PAWN_PROMOTE_BISHOP:
    _pieces[_player_turn][Pieces::BISHOP] ^= move_to_make.pos_to;
    ;
    break;
  case MoveType::PAWN_PROMOTE_KNIGHT:
    _pieces[_player_turn][Pieces::KNIGHT] ^= move_to_make.pos_to;
    break;
  case MoveType::PAWN_MOVE_TWO_SQUARES: {
    if (_player_turn) {
      _last_move_two_squares_push_pawn = (move_to_make.pos_to << 8);
    } else {
      _last_move_two_squares_push_pawn = (move_to_make.pos_to >> 8);
    }

    _pieces[_player_turn][move_to_make.piece_type] ^= move_to_make.pos_to;
    break;
  }
  case MoveType::SHORT_CASTLE_KING_MOVE: {
    _pieces[_player_turn][move_to_make.piece_type] ^= move_to_make.pos_to;

    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_from[_player_turn][1];
    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_to[_player_turn][1];

    _player_turn ^= 1; // change player's turn
    return;
  }
  case MoveType::LONG_CASTLE_KING_MOVE: {
    _pieces[_player_turn][move_to_make.piece_type] ^= move_to_make.pos_to;

    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_from[_player_turn][0];
    _pieces[_player_turn][Pieces::ROOK] ^=
        MoveExplorer::rook_to[_player_turn][0];

    _player_turn ^= 1; // change player's turn
    return;
  }
  default:
    _pieces[_player_turn][move_to_make.piece_type] ^= move_to_make.pos_to;
    break;
  }

  for (std::size_t i{0}; i < ALL_PIECE_TYPES; i++) {
    /* clear the to_pos position
     * where the pawn
     * that moved two squares actually is
     */
    if (undo_move.prev_enpassant_pos == move_to_make.pos_to &&
        move_to_make.move_type == MoveType::REGULAR_PAWN_CAPTURE) {
      if (_player_turn) {
        _pieces[_player_turn ^ 1][i] &= ~(move_to_make.pos_to << 8);
      } else {
        _pieces[_player_turn ^ 1][i] &= ~(move_to_make.pos_to >> 8);
      }
      undo_move.taken_piece = Pieces::PAWN;
    }

    if (_pieces[_player_turn ^ 1][i] & move_to_make.pos_to) {
      undo_move.taken_piece = i;
    }
    _pieces[_player_turn ^ 1][i] &=
        ~move_to_make.pos_to; // clear the to_pos position
  }
  _player_turn ^= 1; // change player's turn
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
        uint64_t all_pieces = 0;
        if (i < 4) {
          all_pieces |= _pieces[turn ^ 1][Pieces::BISHOP];
        } else {
          all_pieces |= _pieces[turn ^ 1][Pieces::ROOK];
        }
        all_pieces |= _pieces[turn ^ 1][QUEEN];
        if (steps == 1) {
          all_pieces |= _pieces[turn ^ 1][KING];
        }

        if (all_pieces & cell_under_investigation) {
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

SquareType Board::getPieceOnSquare(int64_t sq) const {
  for (int32_t turn = 0; turn < 2; turn++) {
    for (int32_t i = 0; i < ALL_PIECE_TYPES; i++) {
      if (_pieces[turn][i] & sq) {
        return SquareType((i << 1) | turn);
      }
    }
  }
  return SquareType::EMPTY;
}

bool Board::getPlayerTurn() const { return _player_turn; }
