#include "move-generator.hpp"
#include "board-inl.hpp"
#include "board.hpp"
#include "move.hpp"

#include <array>

template <std::size_t N>
void generateMoves(const std::array<int8_t, N> &move_shift,
                   const std::array<uint64_t, N> &move_shift_mask, Board &board,
                   const int8_t piece_type, const MoveType move_type,
                   std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();

  uint64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const uint64_t from_bitboard_pos = piece_positions & -piece_positions;
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

      Move move_to_make = Move{from_bitboard_pos, to_bitboard_pos,
                               Pieces{piece_type}, move_type};

      moves.push_back(move_to_make);
    }

    piece_positions ^= from_bitboard_pos;
  }
}

void generatePawnMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();

  static const std::array<MoveType, 4> move_types = {
      MoveType::REGULAR_PAWN_CAPTURE, MoveType::PAWN_MOVE,
      MoveType::REGULAR_PAWN_CAPTURE, MoveType::PAWN_MOVE_TWO_SQUARES};

  static const std::array<MoveType, 4> promotion_types = {
      MoveType::PAWN_PROMOTE_QUEEN, MoveType::PAWN_PROMOTE_ROOK,
      MoveType::PAWN_PROMOTE_BISHOP, MoveType::PAWN_PROMOTE_KNIGHT};

  const std::array<int32_t, 4> move_shift = {turn ? -9 : +7, turn ? -8 : +8,
                                             turn ? -7 : +9, turn ? -16 : +16};
  const std::array<uint64_t, 4> move_shift_mask = {
      MoveGenerator::FILE_A | MoveGenerator::ROW_ONE | MoveGenerator::ROW_SEVEN,
      MoveGenerator::ROW_ONE | MoveGenerator::ROW_SEVEN,
      MoveGenerator::FILE_H | MoveGenerator::ROW_ONE | MoveGenerator::ROW_SEVEN,
      turn ? (MoveGenerator::ROW_ONE_TWO) : (MoveGenerator::ROW_SIX_SEVEN),
  };

  const uint64_t start_row =
      turn ? MoveGenerator::ROW_SIX : MoveGenerator::ROW_TWO;
  const uint64_t finish_row =
      turn ? MoveGenerator::ROW_ONE : MoveGenerator::ROW_SEVEN;
  static const int8_t piece_type = Pieces::PAWN;

  uint64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const uint64_t from_bitboard_pos = piece_positions & -piece_positions;

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

      // check if moving the piece leads to a check to our king
      Move move_to_make = Move{from_bitboard_pos, to_bitboard_pos,
                               Pieces{piece_type}, move_types[i]};
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
          move_to_make.move_type = promotion_type;
          moves.push_back(move_to_make);
        }
      } else {
        moves.push_back(move_to_make);
      }
    }

    piece_positions ^= from_bitboard_pos;
  }
}

template <std::size_t N>
void moveIncrementally(Board &board, const int8_t piece_type,
                       const std::array<int8_t, N> &move_shift,
                       const std::array<uint64_t, N> &move_shift_mask,
                       const MoveType move_type, std::vector<Move> &moves) {

  const bool turn = board.getPlayerTurn();

  uint64_t piece_positions = board.getPiece(piece_type, turn);

  while (piece_positions) {
    const uint64_t from_bitboard_pos = piece_positions & -piece_positions;

    for (std::size_t i{0}; i < N; i++) {
      uint64_t to_bitboard_pos = Board::shiftPosition(
          from_bitboard_pos, move_shift[i], move_shift_mask[i]);

      while (to_bitboard_pos != 0) {
        // check if there is a piece of the same color on this square
        if (board.isCellNotEmpty(to_bitboard_pos, turn)) {
          break;
        }
        bool is_cell_empty = board.isCellNotEmpty(to_bitboard_pos, turn ^ 1);
        // check if moving the piece leads to a check to our king
        Move move_to_make = Move{from_bitboard_pos, to_bitboard_pos,
                                 Pieces{piece_type}, move_type};
        moves.push_back(move_to_make);

        // there is a piece of the opposite color
        if (board.isCellNotEmpty(to_bitboard_pos, turn ^ 1)) {
          break;
        }

        to_bitboard_pos = Board::shiftPosition(to_bitboard_pos, move_shift[i],
                                               move_shift_mask[i]);
      }
    }

    piece_positions ^= from_bitboard_pos;
  }
}

template <std::size_t N>
bool anyCellIsUnderAttack(Board &board,
                          const std::array<uint64_t, N> &cells_to_check) {
  const bool turn = board.getPlayerTurn();

  for (const uint64_t cell_to_check : cells_to_check)
    if (BoardInl::cellIsUnderAttack(board, cell_to_check, turn)) {
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

void generateCastleMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();

  const int8_t row_to_use = turn ? 7 : 0;
  // check short castle
  if (board.checkCastlingRights(turn, 1)) {
    const std::array<uint64_t, 3> cells_to_check = {
        Board::getPositionAsBitboard(row_to_use, 4),
        Board::getPositionAsBitboard(row_to_use, 5),
        Board::getPositionAsBitboard(row_to_use, 6)};

    const std::array<uint64_t, 2> cells_to_check_if_free = {cells_to_check[1],
                                                            cells_to_check[2]};

    if (!anyCellIsUnderAttack(board, cells_to_check) &&
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
    if (!anyCellIsUnderAttack(board, cells_to_check) &&
        cellsAreFree(board, cells_to_check_if_free)) {
      moves.push_back(Move{cells_to_check[2], cells_to_check[0], Pieces::KING,
                           MoveType::LONG_CASTLE_KING_MOVE});
    }
  }
}

void searchKingMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();

  generateCastleMoves(board, moves);

  generateMoves(MoveGenerator::combined_shifts,
                MoveGenerator::combined_shifts_masks, board, Pieces::KING,
                MoveType::REGULAR_KING_MOVE, moves);
}

void searchQueenMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();
  // get diagonal moves
  moveIncrementally(board, Pieces::QUEEN, MoveGenerator::move_diag_shifts,
                    MoveGenerator::move_diag_shifts_masks, MoveType::QUEEN_MOVE,
                    moves);

  // get line moves
  moveIncrementally(board, Pieces::QUEEN, MoveGenerator::move_line_shifts,
                    MoveGenerator::move_line_shifts_masks, MoveType::QUEEN_MOVE,
                    moves);
}

void searchRookMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();

  // get line moves
  moveIncrementally(board, Pieces::ROOK, MoveGenerator::move_line_shifts,
                    MoveGenerator::move_line_shifts_masks, MoveType::ROOK_MOVE,
                    moves);
}

void searchBishopMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();
  // get diagonal moves
  moveIncrementally(board, Pieces::BISHOP, MoveGenerator::move_diag_shifts,
                    MoveGenerator::move_diag_shifts_masks,
                    MoveType::BISHOP_MOVE, moves);
}

void searchKnightMoves(Board &board, std::vector<Move> &moves) {
  const bool turn = board.getPlayerTurn();

  generateMoves(MoveGenerator::knight_move_shifts,
                MoveGenerator::knight_move_shifts_masks, board, Pieces::KNIGHT,
                MoveType::KNIGHT_MOVE, moves);
}

void searchPawnMoves(Board &board, std::vector<Move> &moves) {
  generatePawnMoves(board, moves);
}

void MoveGenerator::generatePseudoLegalMoves(Board &board,
                                             std::vector<Move> &moves) {
  searchKingMoves(board, moves);

  searchQueenMoves(board, moves);

  searchRookMoves(board, moves);

  searchBishopMoves(board, moves);

  searchKnightMoves(board, moves);

  searchPawnMoves(board, moves);
}

namespace MoveGenerator {
uint64_t KING_ATTACK_SQUARES[64];
uint64_t PAWN_ATTACK_SQUARES[64][2];
uint64_t KNIGHT_ATTACK_SQUARES[64];
uint64_t DIAG_ATTACK_SQUARES[64][4];
uint64_t LINE_ATTACK_SQUARES[64][4];
} // namespace MoveGenerator

void MoveGenerator::initAttackTables() {
  uint64_t cell_to_precompute = 0;
  int8_t shift_dir;
  uint64_t mask;
  for (int32_t i = 0; i < 64; i++) {
    uint64_t king_pos = (1LL << i);

    cell_to_precompute = 0;
    // check king colission
    for (std::size_t i = 0; i < MoveGenerator::combined_shifts.size(); i++) {
      shift_dir = MoveGenerator::combined_shifts[i];
      mask = MoveGenerator::combined_shifts_masks[i];

      cell_to_precompute |= Board::shiftPosition(king_pos, shift_dir, mask);
    }

    KING_ATTACK_SQUARES[i] = cell_to_precompute;
  }

  for (int32_t i = 0; i < 64; i++) {
    uint64_t knight_pos = (1LL << i);

    cell_to_precompute = 0;
    // check king colission
    for (std::size_t i = 0; i < MoveGenerator::knight_move_shifts.size(); i++) {
      shift_dir = MoveGenerator::knight_move_shifts[i];
      mask = MoveGenerator::knight_move_shifts_masks[i];

      cell_to_precompute |= Board::shiftPosition(knight_pos, shift_dir, mask);
    }

    KNIGHT_ATTACK_SQUARES[i] = cell_to_precompute;
  }

  for (int32_t i = 0; i < 64; i++) {
    uint64_t start_pos = (1LL << i);
    uint64_t cur_cell;

    for (std::size_t j = 0; j < MoveGenerator::move_diag_shifts.size(); j++) {
      cell_to_precompute = 0;

      shift_dir = MoveGenerator::move_diag_shifts[j];
      mask = MoveGenerator::move_diag_shifts_masks[j];
      cur_cell = Board::shiftPosition(start_pos, shift_dir, mask);

      while (cur_cell != 0) {
        cell_to_precompute |= cur_cell;

        cur_cell = Board::shiftPosition(cur_cell, shift_dir, mask);
      }
      DIAG_ATTACK_SQUARES[i][j] = cell_to_precompute;
    }
  }

  for (int32_t i = 0; i < 64; i++) {
    uint64_t start_pos = (1LL << i);
    uint64_t cur_cell;

    for (std::size_t j = 0; j < MoveGenerator::move_line_shifts.size(); j++) {
      cell_to_precompute = 0;

      shift_dir = MoveGenerator::move_line_shifts[j];
      mask = MoveGenerator::move_line_shifts_masks[j];
      cur_cell = Board::shiftPosition(start_pos, shift_dir, mask);

      while (cur_cell != 0) {
        cell_to_precompute |= cur_cell;

        cur_cell = Board::shiftPosition(cur_cell, shift_dir, mask);
      }
      LINE_ATTACK_SQUARES[i][j] = cell_to_precompute;
    }
  }

  for (int32_t i = 0; i < 64; i++) {
    uint64_t start_pos = (1LL << i);

    for (int32_t turn = 0; turn < 2; turn++) {
      // check for pawn checks
      uint64_t cell_under_investigation =
          Board::shiftPosition(
              start_pos, turn ? -9 : +7,
              MoveGenerator::FILE_A |
                  (turn ? MoveGenerator::ROW_ONE : MoveGenerator::ROW_SEVEN)) |
          Board::shiftPosition(
              start_pos, turn ? -7 : +9,
              MoveGenerator::FILE_H |
                  (turn ? MoveGenerator::ROW_ONE : MoveGenerator::ROW_SEVEN));

      PAWN_ATTACK_SQUARES[i][turn] = cell_under_investigation;
    }
  }
}
