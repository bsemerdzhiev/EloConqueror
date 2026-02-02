#include "search.hpp"
#include "board.hpp"
#include "move.hpp"
#include "undo_move.hpp"

#include <array>
#include <bit>

template <std::size_t N>
void generateMoves(const std::array<int8_t, N> &move_shift,
                   const std::array<uint64_t, N> &move_shift_mask, Board &board,
                   const int8_t piece_type, const bool turn,
                   const MoveType move_type, std::vector<Move> &moves) {
  uint64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const int8_t position = std::__countr_zero(piece_positions);

    const uint64_t from_bitboard_pos = (1LL << position);
    uint64_t to_bitboard_pos = from_bitboard_pos;

    for (size_t i{0}; i < N; i++) {
      to_bitboard_pos = Board::shiftPosition(from_bitboard_pos, move_shift[i],
                                             move_shift_mask[i]);

      // piece is outside the bounds
      if (to_bitboard_pos == 0)
        continue;

      // check if there is a piece of the same color on this square
      if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
        continue;
      }

      // check if moving the piece leads to a check to our king
      UndoMove undo_move;
      board.makeMove(from_bitboard_pos, to_bitboard_pos, piece_type, turn,
                     move_type, undo_move);

      if (board.isUnderCheck(board.getPiece(Pieces::KING, turn), turn)) {
        board.unmakeMove(undo_move);
        continue;
      }
      board.unmakeMove(undo_move);

      moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                           Pieces{piece_type}, move_type});
    }

    piece_positions ^= (uint64_t{1} << position);
  }
}

void generatePawnMoves(Board &board, const bool turn,
                       std::vector<Move> &moves) {
  static const std::array<MoveType, 4> move_types = {
      MoveType::REGULAR_PAWN_CAPTURE, MoveType::PAWN_MOVE,
      MoveType::REGULAR_PAWN_CAPTURE, MoveType::PAWN_MOVE_TWO_SQUARES};

  static const std::array<MoveType, 4> promotion_types = {
      MoveType::PAWN_PROMOTE_QUEEN, MoveType::PAWN_PROMOTE_ROOK,
      MoveType::PAWN_PROMOTE_BISHOP, MoveType::PAWN_PROMOTE_KNIGHT};

  const std::array<int32_t, 4> move_shift = {turn ? -9 : +7, turn ? -8 : +8,
                                             turn ? -7 : +9, turn ? -16 : +16};
  const std::array<uint64_t, 4> move_shift_mask = {
      MoveExplorer::FILE_A | MoveExplorer::ROW_ONE | MoveExplorer::ROW_SEVEN,
      MoveExplorer::ROW_ONE | MoveExplorer::ROW_SEVEN,
      MoveExplorer::FILE_H | MoveExplorer::ROW_ONE | MoveExplorer::ROW_SEVEN,
      turn ? (MoveExplorer::ROW_ONE_TWO) : (MoveExplorer::ROW_SIX_SEVEN),
  };

  const uint64_t start_row =
      turn ? MoveExplorer::ROW_SIX : MoveExplorer::ROW_TWO;
  const uint64_t finish_row =
      turn ? MoveExplorer::ROW_ONE : MoveExplorer::ROW_SEVEN;
  static const int8_t piece_type = Pieces::PAWN;

  uint64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const int8_t position = std::__countr_zero(piece_positions);

    const uint64_t from_bitboard_pos = (1LL << position);

    for (std::size_t i{0}; i < move_types.size(); i++) {
      const uint64_t to_bitboard_pos = Board::shiftPosition(
          from_bitboard_pos, move_shift[i], move_shift_mask[i]);

      if (to_bitboard_pos == 0) {
        continue;
      }

      // check if there is a piece of the same color on this square
      if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
        continue;
      }

      UndoMove undo_move;
      // check if moving the piece leads to a check to our king
      board.makeMove(from_bitboard_pos, to_bitboard_pos, piece_type, turn,
                     move_types[i], undo_move);

      if (board.isUnderCheck(board.getPiece(Pieces::KING, turn), turn)) {
        board.unmakeMove(undo_move);
        continue;
      }
      board.unmakeMove(undo_move);

      bool no_blockers_check = true;

      if (i == 3) {
        uint64_t in_between_cell = to_bitboard_pos;
        if (turn) {
          in_between_cell <<= 8;
        } else {
          in_between_cell >>= 8;
        }
        if ((from_bitboard_pos & start_row) == 0 ||
            board.isCellNotEmpty(in_between_cell, 0) ||
            board.isCellNotEmpty(in_between_cell, 1)) {
          continue;
        }
      }

      // this means there must an enemy piece there to capture
      if ((i == 0 || i == 2) &&
          ((!board.isCellNotEmpty(to_bitboard_pos, turn ^ 1) &&
            !board.isEnPassant(to_bitboard_pos, turn)))) {
        continue;
      } else if ((i == 1 || i == 3) &&
                 board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
        continue;
      }

      if ((to_bitboard_pos & finish_row) != 0) {
        for (const auto &promotion_type : promotion_types) {
          moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                               Pieces{piece_type}, promotion_type});
        }
      } else {
        moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                             Pieces{piece_type}, move_types[i]});
      }
    }

    piece_positions ^= (uint64_t{1} << position);
  }
}

template <std::size_t N>
void moveIncrementally(Board &board, const bool turn, const int8_t piece_type,
                       const std::array<int8_t, N> &move_shift,
                       const std::array<uint64_t, N> &move_shift_mask,
                       const MoveType move_type, std::vector<Move> &moves) {

  uint64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const int8_t position = std::__countr_zero(piece_positions);

    const uint64_t from_bitboard_pos = (1LL << position);

    for (std::size_t i{0}; i < N; i++) {
      uint64_t to_bitboard_pos = Board::shiftPosition(
          from_bitboard_pos, move_shift[i], move_shift_mask[i]);

      while (to_bitboard_pos != 0) {
        // check if there is a piece of the same color on this square
        if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
          break;
        }

        // check if moving the piece leads to a check to our king
        UndoMove undo_move;
        board.makeMove(from_bitboard_pos, to_bitboard_pos, piece_type, turn,
                       move_type, undo_move);

        if (board.isUnderCheck(board.getPiece(Pieces::KING, turn), turn)) {
          // there is a piece of the opposite color
          if (board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
            board.unmakeMove(undo_move);
            break;
          }

          board.unmakeMove(undo_move);
          to_bitboard_pos = Board::shiftPosition(to_bitboard_pos, move_shift[i],
                                                 move_shift_mask[i]);
          continue;
        }
        board.unmakeMove(undo_move);
        moves.push_back(Move{from_bitboard_pos, to_bitboard_pos,
                             Pieces{piece_type}, move_type});

        // there is a piece of the opposite color
        if (board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
          break;
        }

        to_bitboard_pos = Board::shiftPosition(to_bitboard_pos, move_shift[i],
                                               move_shift_mask[i]);
      }
    }

    piece_positions ^= (uint64_t{1} << position);
  }
}

void MoveExplorer::searchAllMoves(Board &board, const bool turn,
                                  std::vector<Move> &moves) {
  searchKingMoves(board, turn, moves);

  searchQueenMoves(board, turn, moves);

  searchRookMoves(board, turn, moves);

  searchBishopMoves(board, turn, moves);

  searchKnightMoves(board, turn, moves);

  searchPawnMoves(board, turn, moves);
}

template <std::size_t N>
bool anyCellIsUnderAttack(Board &board,
                          const std::array<uint64_t, N> &cells_to_check,
                          const bool turn) {

  for (const uint64_t cell_to_check : cells_to_check)
    if (board.isUnderCheck(cell_to_check, turn)) {
      return true;
    }
  return false;
}

template <std::size_t N>
bool cellsAreFree(Board &board, const std::array<uint64_t, N> &cells_to_check) {
  for (const uint64_t cell_to_check : cells_to_check) {
    if (board.isCellNotEmpty(cell_to_check, 0) ||
        board.isCellNotEmpty(cell_to_check, 1)) {
      return false;
    }
  }

  return true;
}

void generateCastleMoves(Board &board, bool turn, std::vector<Move> &moves) {
  // generate all attacked squares by the enemy
  // std::vector<Move> attacked_squares;
  // attacked_squares.resize(2);

  // MoveExplorer::searchAllMoves(board, turn ^ 1, attacked_squares);

  const int8_t row_to_use = turn ? 7 : 0;
  // check short castle
  if (board.checkCastlingRights(turn, 1)) {
    const std::array<uint64_t, 3> cells_to_check = {
        Board::getPositionAsBitboard(row_to_use, 4),
        Board::getPositionAsBitboard(row_to_use, 5),
        Board::getPositionAsBitboard(row_to_use, 6)};

    const std::array<uint64_t, 2> cells_to_check_if_free = {cells_to_check[1],
                                                            cells_to_check[2]};

    if (!anyCellIsUnderAttack(board, cells_to_check, turn) &&
        cellsAreFree(board, cells_to_check_if_free)) {
      moves.push_back(Move{cells_to_check[0], cells_to_check[2], Pieces::KING,
                           MoveType::SHORT_CASTLE_KING_MOVE});
    }
  }
  if (board.checkCastlingRights(turn, 0)) {
    const std::array<uint64_t, 3> cells_to_check = {
        Board::getPositionAsBitboard(row_to_use, 2),
        Board::getPositionAsBitboard(row_to_use, 3),
        Board::getPositionAsBitboard(row_to_use, 4),
    };
    const std::array<uint64_t, 3> cells_to_check_if_free = {
        Board::getPositionAsBitboard(row_to_use, 1), cells_to_check[0],
        cells_to_check[1]};
    if (!anyCellIsUnderAttack(board, cells_to_check, turn) &&
        cellsAreFree(board, cells_to_check_if_free)) {
      moves.push_back(Move{cells_to_check[2], cells_to_check[0], Pieces::KING,
                           MoveType::LONG_CASTLE_KING_MOVE});
    }
  }
}

void MoveExplorer::searchKingMoves(Board &board, const bool turn,
                                   std::vector<Move> &moves) {

  generateCastleMoves(board, turn, moves);

  generateMoves(combined_shifts, combined_shifts_masks, board, Pieces::KING,
                turn, MoveType::REGULAR_KING_MOVE, moves);
}

void MoveExplorer::searchQueenMoves(Board &board, const bool turn,
                                    std::vector<Move> &moves) {
  // get diagonal moves
  moveIncrementally(board, turn, Pieces::QUEEN, MoveExplorer::move_diag_shifts,
                    MoveExplorer::move_diag_shifts_masks, MoveType::QUEEN_MOVE,
                    moves);

  // get line moves
  moveIncrementally(board, turn, Pieces::QUEEN, MoveExplorer::move_line_shifts,
                    MoveExplorer::move_line_shifts_masks, MoveType::QUEEN_MOVE,
                    moves);
}

void MoveExplorer::searchRookMoves(Board &board, const bool turn,
                                   std::vector<Move> &moves) {

  // get line moves
  moveIncrementally(board, turn, Pieces::ROOK, MoveExplorer::move_line_shifts,
                    MoveExplorer::move_line_shifts_masks, MoveType::ROOK_MOVE,
                    moves);
}

void MoveExplorer::searchBishopMoves(Board &board, const bool turn,
                                     std::vector<Move> &moves) {
  // get diagonal moves
  moveIncrementally(board, turn, Pieces::BISHOP, MoveExplorer::move_diag_shifts,
                    MoveExplorer::move_diag_shifts_masks, MoveType::BISHOP_MOVE,
                    moves);
}

void MoveExplorer::searchKnightMoves(Board &board, const bool turn,
                                     std::vector<Move> &moves) {
  generateMoves(knight_move_shifts, knight_move_shifts_masks, board,
                Pieces::KNIGHT, turn, MoveType::KNIGHT_MOVE, moves);
}

void MoveExplorer::searchPawnMoves(Board &board, const bool turn,
                                   std::vector<Move> &moves) {
  generatePawnMoves(board, turn, moves);
}
