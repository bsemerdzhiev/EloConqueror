#include "search.hpp"
#include "board.hpp"
#include "move.hpp"

#include <bit>

std::vector<Move> generateMoves(const int32_t move_row[],
                                const int32_t move_col[],
                                const size_t move_size, const Board &board,
                                const bool should_move, const int8_t piece_type,
                                const bool turn) {
  int64_t piece_positions = board.getPiece(piece_type, turn);

  std::vector<Move> moves;
  while (piece_positions) {
    int32_t position = std::__countr_zero(piece_positions);

    int32_t pos_row = position / Board::BOARD_ROWS;
    int32_t pos_col = position % Board::BOARD_ROWS;

    for (size_t i{0}; i < move_size; i++) {
      int32_t new_pos_row = pos_row + move_row[i];
      int32_t new_pos_col = pos_col + move_col[i];

      // check if the piece is outside
      if (new_pos_row >= 0 && new_pos_row < Board::BOARD_ROWS &&
          new_pos_col >= 0 && new_pos_col < Board::BOARD_COLS) {
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
                                         piece_type, turn);

        if (new_board.isUnderCheck(turn ^ 1)) {
          continue;
        }
      }

      moves.push_back(Move{from_bitboard_pos, to_bitboard_pos});
    }

    piece_positions ^= (int64_t{1} << position);
  }

  return moves;
}

std::vector<Move> generatePawnMoves(const Board &board, const bool should_move,
                                    const bool turn) {
  std::vector<Move> moves;

  const int32_t move_row[4] = {turn ? +1 : -1, turn ? +1 : -1, turn ? +1 : -1,
                               turn ? +2 : -2};
  const int32_t move_col[4] = {-1, 0, +1, 0};
  const int32_t start_row = turn ? 1 : 6;
  const int8_t piece_type = 5;

  int64_t piece_positions = board.getPiece(5, turn);

  while (piece_positions) {
    int32_t position = std::__countr_zero(piece_positions);

    int32_t pawn_row = position / Board::BOARD_ROWS;
    int32_t pawn_col = position % Board::BOARD_ROWS;

    for (std::size_t i{0}; i < 4; i++) {
      int32_t new_pos_row = pawn_row + move_row[i];
      int32_t new_pos_col = pawn_col + move_col[i];

      // check if the piece is outside
      if (new_pos_row >= 0 && new_pos_row < Board::BOARD_ROWS &&
          new_pos_col >= 0 && new_pos_col < Board::BOARD_COLS) {
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
        Board new_board =
            board.makeMove(position, to_bitboard_pos, piece_type, turn);

        if (new_board.isUnderCheck(turn ^ 1)) {
          continue;
        }
      }

      // this means there must an enemy piece there
      if ((i == 0 || i == 2) &&
          !board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
        continue;
      } else if (i == 3 && pawn_row != start_row) {
        continue;
      }

      moves.push_back(Move{from_bitboard_pos, to_bitboard_pos});
    }

    piece_positions ^= (int64_t{1} << position);
  }
  return moves;
}

std::vector<Move> moveIncrementally(const Board &board, const bool should_move,
                                    const bool turn, const int8_t piece_type,
                                    const int32_t move_row[],
                                    const int32_t move_col[],
                                    const size_t move_size) {
  std::vector<Move> moves;

  int64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    int32_t position = std::__countr_zero(piece_positions);

    int32_t start_row = position / Board::BOARD_ROWS;
    int32_t start_col = position % Board::BOARD_ROWS;

    int64_t from_bitboard_pos =
        Board::getPositionAsBitboard(start_row, start_col);

    for (std::size_t i{0}; i < move_size; i++) {
      int32_t pos_row = start_row;
      int32_t pos_col = start_col;

      pos_row += move_row[i];
      pos_col += move_col[i];

      while (pos_row >= 0 && pos_col >= 0 && pos_row < Board::BOARD_ROWS &&
             pos_col < Board::BOARD_COLS) {

        int64_t to_bitboard_pos =
            Board::getPositionAsBitboard(pos_row, pos_col);

        pos_row += move_row[i];
        pos_col += move_col[i];

        // check if there is a piece of the same color on this square
        if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
          break;
        }

        if (should_move) {
          // check if moving the piece leads to a check to our king
          Board new_board = board.makeMove(from_bitboard_pos, to_bitboard_pos,
                                           piece_type, turn);

          if (new_board.isUnderCheck(turn ^ 1)) {
            continue;
          }
        }

        moves.push_back(Move{from_bitboard_pos, to_bitboard_pos});
        // there is a piece of the opposite color
        if (board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
          break;
        }
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

std::vector<Move> MoveExplorer::searchKingMoves(const Board &board,
                                                const bool turn,
                                                const bool should_move) {
  std::vector<Move> moves;

  const int32_t move_row[8] = {-1, -1, -1, 0, +1, +1, +1, 0};
  const int32_t move_col[8] = {-1, 0, +1, +1, +1, 0, -1, -1};

  // TODO check for castling

  return generateMoves(move_row, move_col, 4, board, should_move, 0, turn);
}

std::vector<Move> MoveExplorer::searchQueenMoves(const Board &board,
                                                 const bool turn,
                                                 const bool should_move) {
  std::vector<Move> moves;

  // get diagonal moves
  for (const auto &new_move : moveIncrementally(
           board, should_move, turn, 1, MoveExplorer::move_row_diag,
           MoveExplorer::move_col_diag, 4)) {
    moves.push_back(new_move);
  }

  // get line moves
  for (const auto &new_move : moveIncrementally(
           board, should_move, turn, 1, MoveExplorer::move_row_line,
           MoveExplorer::move_col_line, 4)) {
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
           board, should_move, turn, 2, MoveExplorer::move_row_line,
           MoveExplorer::move_col_line, 4)) {
    moves.push_back(new_move);
  }
  return moves;
}

std::vector<Move> MoveExplorer::searchBishopMoves(const Board &board,
                                                  const bool turn,
                                                  const bool should_move) {
  std::vector<Move> moves;

  // get diagonal moves
  for (const auto &new_move : moveIncrementally(
           board, should_move, turn, 3, MoveExplorer::move_row_diag,
           MoveExplorer::move_col_diag, 4)) {
    moves.push_back(new_move);
  }
  return moves;
}

std::vector<Move> MoveExplorer::searchKnightMoves(const Board &board,
                                                  const bool turn,
                                                  const bool should_move) {
  std::vector<Move> moves;

  const int32_t move_row[8] = {-2, -2, -1, 1, +2, +2, +1, -1};
  const int32_t move_col[8] = {-1, +1, +2, +2, +1, -1, -2, -2};

  return generateMoves(move_row, move_col, 4, board, should_move, 5, turn);
}

std::vector<Move> MoveExplorer::searchPawnMoves(const Board &board,
                                                const bool turn,
                                                const bool should_move) {
  std::vector<Move> moves;

  // TODO

  return moves;
}
