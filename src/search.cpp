#include "search.hpp"
#include "board.hpp"
#include "move.hpp"

#include <array>
#include <bit>
#include <iostream>

template <std::size_t N>
std::vector<Move> generateMoves(const std::array<int32_t, N> &move_row,
                                const std::array<int32_t, N> &move_col,
                                const Board &board, const bool should_move,
                                const int8_t piece_type, const bool turn,
                                const MoveType move_type) {
  int64_t piece_positions = board.getPiece(piece_type, turn);

  std::vector<Move> moves;
  while (piece_positions) {
    int32_t position = std::__countr_zero(piece_positions);

    int32_t pos_row = position / Board::BOARD_ROWS;
    int32_t pos_col = position % Board::BOARD_ROWS;

    for (size_t i{0}; i < N; i++) {
      int32_t new_pos_row = pos_row + move_row[i];
      int32_t new_pos_col = pos_col + move_col[i];

      // check if the piece is outside
      if (new_pos_row < 0 || new_pos_row >= Board::BOARD_ROWS ||
          new_pos_col < 0 || new_pos_col >= Board::BOARD_COLS) {
        continue;
      }

      int64_t from_bitboard_pos =
          Board::getPositionAsBitboard(pos_row, pos_col);
      int64_t to_bitboard_pos =
          Board::getPositionAsBitboard(new_pos_row, new_pos_col);

      // check if there is a piece of the same color on this square
      if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
        continue;
      }

      if (should_move) {
        // check if moving the piece leads to a check to our king
        Board new_board = board.makeMove(from_bitboard_pos, to_bitboard_pos,
                                         piece_type, turn, move_type);

        if (new_board.isUnderCheck(turn)) {
          continue;
        }
      }

      moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                           Pieces{piece_type}, move_type});
    }

    piece_positions ^= (int64_t{1} << position);
  }

  return moves;
}

std::vector<Move> generatePawnMoves(const Board &board, const bool should_move,
                                    const bool turn) {
  std::vector<Move> moves;

  const std::array<MoveType, 4> move_types = {
      MoveType::REGULAR_PAWN_CAPTURE, MoveType::PAWN_MOVE,
      MoveType::REGULAR_PAWN_CAPTURE, MoveType::PAWN_MOVE_TWO_SQUARES};

  const std::array<MoveType, 4> promotion_types = {
      MoveType::PAWN_PROMOTE_QUEEN, MoveType::PAWN_PROMOTE_ROOK,
      MoveType::PAWN_PROMOTE_BISHOP, MoveType::PAWN_PROMOTE_KNIGHT};

  const std::array<int32_t, 4> move_row = {turn ? -1 : +1, turn ? -1 : +1,
                                           turn ? -1 : +1, turn ? -2 : +2};
  const std::array<int32_t, 4> move_col = {-1, 0, +1, 0};
  const int32_t start_row = turn ? Board::BOARD_ROWS - 2 : 1;
  const int32_t finish_row = turn ? 0 : Board::BOARD_ROWS - 1;
  const int8_t piece_type = Pieces::PAWN;

  int64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    int32_t position = std::__countr_zero(piece_positions);

    int32_t pawn_row = position / Board::BOARD_ROWS;
    int32_t pawn_col = position % Board::BOARD_ROWS;

    for (std::size_t i{0}; i < move_types.size(); i++) {
      if (!should_move && (i == 1 || i == 3)) {
        continue;
      }

      int32_t new_pos_row = pawn_row + move_row[i];
      int32_t new_pos_col = pawn_col + move_col[i];

      // check if the piece is outside
      if (new_pos_row < 0 || new_pos_row >= Board::BOARD_ROWS ||
          new_pos_col < 0 || new_pos_col >= Board::BOARD_COLS) {
        continue;
      }

      int64_t from_bitboard_pos =
          Board::getPositionAsBitboard(pawn_row, pawn_col);
      int64_t to_bitboard_pos =
          Board::getPositionAsBitboard(new_pos_row, new_pos_col);

      // check if there is a piece of the same color on this square
      if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
        continue;
      }

      // check if moving the piece leads to a check to our king
      if (should_move) {
        Board new_board = board.makeMove(from_bitboard_pos, to_bitboard_pos,
                                         piece_type, turn, move_types[i]);

        if (new_board.isUnderCheck(turn)) {
          continue;
        }
      }

      bool no_blockers_check = true;

      if (i == 3) {
        int64_t in_between_cell = to_bitboard_pos;
        if (turn) {
          in_between_cell <<= 8;
        } else {
          in_between_cell >>= 8;
        }
        if (pawn_row != start_row || board.isCellNotEmpty(in_between_cell, 0) ||
            board.isCellNotEmpty(in_between_cell, 1)) {
          continue;
        }
      }

      // this means there must an enemy piece there to capture
      if ((i == 0 || i == 2) &&
          (should_move && (!board.isCellNotEmpty(to_bitboard_pos, turn ^ 1) &&
                           !board.isEnPassant(to_bitboard_pos, turn)))) {
        continue;
      } else if ((i == 1 || i == 3) &&
                 board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
        continue;
      }

      if (new_pos_row == finish_row) {
        for (const auto &promotion_type : promotion_types) {
          moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                               Pieces{piece_type}, promotion_type});
        }
      } else {

        moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                             Pieces{piece_type}, move_types[i]});
      }
    }

    piece_positions ^= (int64_t{1} << position);
  }
  return moves;
}

template <std::size_t N>
std::vector<Move> moveIncrementally(const Board &board, const bool should_move,
                                    const bool turn, const int8_t piece_type,
                                    const std::array<int32_t, N> &move_row,
                                    const std::array<int32_t, N> &move_col,
                                    const MoveType move_type) {
  std::vector<Move> moves;

  int64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const int32_t position = std::__countr_zero(piece_positions);

    const int32_t start_row = position / Board::BOARD_ROWS;
    const int32_t start_col = position % Board::BOARD_ROWS;

    const int64_t from_bitboard_pos =
        Board::getPositionAsBitboard(start_row, start_col);

    for (std::size_t i{0}; i < N; i++) {
      int32_t pos_row = start_row;
      int32_t pos_col = start_col;

      pos_row += move_row[i];
      pos_col += move_col[i];

      while (pos_row >= 0 && pos_col >= 0 && pos_row < Board::BOARD_ROWS &&
             pos_col < Board::BOARD_COLS) {

        const int64_t to_bitboard_pos =
            Board::getPositionAsBitboard(pos_row, pos_col);

        // check if there is a piece of the same color on this square
        if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
          break;
        }

        if (should_move) {
          // check if moving the piece leads to a check to our king
          Board new_board = board.makeMove(from_bitboard_pos, to_bitboard_pos,
                                           piece_type, turn, move_type);

          if (new_board.isUnderCheck(turn)) {
            // there is a piece of the opposite color
            if (board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
              break;
            }

            pos_row += move_row[i];
            pos_col += move_col[i];
            continue;
          }
        }
        moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                             Pieces{piece_type}, move_type});

        // there is a piece of the opposite color
        if (board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
          break;
        }

        pos_row += move_row[i];
        pos_col += move_col[i];
      }
    }

    piece_positions ^= (int64_t{1} << position);
  }
  return moves;
}

std::vector<Move> MoveExplorer::searchAllMoves(const Board &board,
                                               const bool turn,
                                               const bool should_move) {
  std::vector<Move> moves;

  for (const auto &new_move : searchKingMoves(board, turn, should_move)) {
    moves.push_back(new_move);
  }

  for (const auto &new_move : searchQueenMoves(board, turn, should_move)) {
    moves.push_back(new_move);
  }

  for (const auto &new_move : searchRookMoves(board, turn, should_move)) {
    moves.push_back(new_move);
  }

  for (const auto &new_move : searchBishopMoves(board, turn, should_move)) {
    moves.push_back(new_move);
  }

  for (const auto &new_move : searchKnightMoves(board, turn, should_move)) {
    moves.push_back(new_move);
  }

  for (const auto &new_move : searchPawnMoves(board, turn, should_move)) {
    moves.push_back(new_move);
  }

  return moves;
}

template <std::size_t N>
bool anyCellIsUnderAttack(const std::vector<Move> &attacked_squares,
                          const std::array<int64_t, N> &cells_to_check) {

  for (const auto &under_attack : attacked_squares) {
    for (const int64_t cell_to_check : cells_to_check)
      if (cell_to_check == under_attack.pos_to) {
        return true;
      }
  }
  return false;
}

template <std::size_t N>
bool cellsAreFree(const Board &board,
                  const std::array<int64_t, N> &cells_to_check) {
  for (const int64_t cell_to_check : cells_to_check) {
    if (board.isCellNotEmpty(cell_to_check, 0) ||
        board.isCellNotEmpty(cell_to_check, 1)) {
      return false;
    }
  }

  return true;
}

std::vector<Move> generateCastleMoves(const Board &board, bool turn) {
  std::vector<Move> moves;

  // generate all attacked squares by the enemy
  std::vector<Move> attacked_squares =
      MoveExplorer::searchAllMoves(board, turn ^ 1, false);

  const int8_t row_to_use = turn ? 7 : 0;
  // check short castle
  if (board.checkCastlingRights(turn, 1)) {
    const std::array<int64_t, 3> cells_to_check = {
        Board::getPositionAsBitboard(row_to_use, 4),
        Board::getPositionAsBitboard(row_to_use, 5),
        Board::getPositionAsBitboard(row_to_use, 6)};

    const std::array<int64_t, 2> cells_to_check_if_free = {cells_to_check[1],
                                                           cells_to_check[2]};

    if (!anyCellIsUnderAttack(attacked_squares, cells_to_check) &&
        cellsAreFree(board, cells_to_check_if_free)) {
      moves.push_back(Move{cells_to_check[0], cells_to_check[2], Pieces::KING,
                           MoveType::SHORT_CASTLE_KING_MOVE});
    }
  }
  if (board.checkCastlingRights(turn, 0)) {
    const std::array<int64_t, 3> cells_to_check = {
        Board::getPositionAsBitboard(row_to_use, 2),
        Board::getPositionAsBitboard(row_to_use, 3),
        Board::getPositionAsBitboard(row_to_use, 4),
    };
    const std::array<int64_t, 3> cells_to_check_if_free = {
        Board::getPositionAsBitboard(row_to_use, 1), cells_to_check[0],
        cells_to_check[1]};
    if (!anyCellIsUnderAttack(attacked_squares, cells_to_check) &&
        cellsAreFree(board, cells_to_check_if_free)) {
      moves.push_back(Move{cells_to_check[2], cells_to_check[0], Pieces::KING,
                           MoveType::LONG_CASTLE_KING_MOVE});
    }
  }

  return moves;
}

std::vector<Move> MoveExplorer::searchKingMoves(const Board &board,
                                                const bool turn,
                                                const bool should_move) {

  std::vector<Move> moves;

  const std::array<int32_t, 8> move_row = {-1, -1, -1, 0, +1, +1, +1, 0};
  const std::array<int32_t, 8> move_col = {-1, 0, +1, +1, +1, 0, -1, -1};

  if (should_move) {
    for (const auto &cur_move : generateCastleMoves(board, turn)) {
      moves.push_back(cur_move);
    }
  }

  for (const auto &cur_move :
       generateMoves(move_row, move_col, board, should_move, Pieces::KING, turn,
                     MoveType::REGULAR_KING_MOVE)) {
    moves.push_back(cur_move);
  }

  return moves;
}

std::vector<Move> MoveExplorer::searchQueenMoves(const Board &board,
                                                 const bool turn,
                                                 const bool should_move) {
  std::vector<Move> moves;

  // get diagonal moves
  for (const auto &new_move : moveIncrementally(
           board, should_move, turn, Pieces::QUEEN, MoveExplorer::move_row_diag,
           MoveExplorer::move_col_diag, MoveType::QUEEN_MOVE)) {
    moves.push_back(new_move);
  }

  // get line moves
  for (const auto &new_move : moveIncrementally(
           board, should_move, turn, Pieces::QUEEN, MoveExplorer::move_row_line,
           MoveExplorer::move_col_line, MoveType::QUEEN_MOVE)) {
    moves.push_back(new_move);
  }
  return moves;
}

std::vector<Move> MoveExplorer::searchRookMoves(const Board &board,
                                                const bool turn,
                                                const bool should_move) {

  std::vector<Move> moves;

  // get line moves
  for (const auto &new_move : moveIncrementally(
           board, should_move, turn, Pieces::ROOK, MoveExplorer::move_row_line,
           MoveExplorer::move_col_line, MoveType::ROOK_MOVE)) {
    moves.push_back(new_move);
  }
  return moves;
}

std::vector<Move> MoveExplorer::searchBishopMoves(const Board &board,
                                                  const bool turn,
                                                  const bool should_move) {
  std::vector<Move> moves;

  // get diagonal moves
  for (const auto &new_move :
       moveIncrementally(board, should_move, turn, Pieces::BISHOP,
                         MoveExplorer::move_row_diag,
                         MoveExplorer::move_col_diag, MoveType::BISHOP_MOVE)) {
    moves.push_back(new_move);
  }
  return moves;
}

std::vector<Move> MoveExplorer::searchKnightMoves(const Board &board,
                                                  const bool turn,
                                                  const bool should_move) {
  const std::array<int32_t, 8> move_row = {-2, -2, -1, 1, +2, +2, +1, -1};
  const std::array<int32_t, 8> move_col = {-1, +1, +2, +2, +1, -1, -2, -2};

  return generateMoves(move_row, move_col, board, should_move, Pieces::KNIGHT,
                       turn, MoveType::KNIGHT_MOVE);
}

std::vector<Move> MoveExplorer::searchPawnMoves(const Board &board,
                                                const bool turn,
                                                const bool should_move) {
  return generatePawnMoves(board, should_move, turn);
}
