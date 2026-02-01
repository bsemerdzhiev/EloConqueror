#ifndef SEARCH_H
#define SEARCH_H

#include "board.hpp"
#include "move.hpp"

#include <array>
#include <vector>

namespace MoveExplorer {
void searchAllMoves(const Board &board, const bool turn,
                    std::vector<Move> &moves);
void searchKingMoves(const Board &board, const bool turn,
                     std::vector<Move> &moves);
void searchQueenMoves(const Board &board, const bool turn,
                      std::vector<Move> &moves);
void searchRookMoves(const Board &board, const bool turn,
                     std::vector<Move> &moves);
void searchBishopMoves(const Board &board, const bool turn,
                       std::vector<Move> &moves);
void searchKnightMoves(const Board &board, const bool turn,
                       std::vector<Move> &moves);
void searchPawnMoves(const Board &board, const bool turn,
                     std::vector<Move> &moves);

constexpr uint64_t FILE_A = 0x0101010101010101ULL;
constexpr uint64_t FILE_B = 0x0202020202020202ULL;
constexpr uint64_t FILE_G = 0x4040404040404040ULL;
constexpr uint64_t FILE_H = 0x8080808080808080ULL;
constexpr uint64_t ROW_ONE = 0x00000000000000FFULL;
constexpr uint64_t ROW_TWO = 0x000000000000FF00ULL;
constexpr uint64_t ROW_SIX = 0x00FF000000000000ULL;
constexpr uint64_t ROW_SEVEN = 0xFF00000000000000ULL;

constexpr std::array<int32_t, 4> move_row_diag = {-1, -1, +1, +1};
constexpr std::array<int32_t, 4> move_col_diag = {-1, +1, -1, +1};

constexpr std::array<int8_t, 4> move_diag_shifts = {-9, -7, +7, +9};
constexpr std::array<uint64_t, 4> move_diag_shifts_masks = {
    FILE_A | ROW_ONE, FILE_H | ROW_ONE, FILE_A | ROW_SEVEN, FILE_H | ROW_SEVEN};

constexpr std::array<int32_t, 4> move_row_line = {0, 0, -1, +1};
constexpr std::array<int32_t, 4> move_col_line = {-1, +1, 0, 0};

// constexpr std::array<int8_t, 8> combined_rows = {-1, -1, +1, +1, 0, 0, -1,
// +1}; constexpr std::array<int8_t, 8> combined_cols = {-1, +1, -1, +1, -1, +1,
// 0, 0};

constexpr std::array<int8_t, 8> combined_shifts = {-9, -7, +7, +9,
                                                   -1, +1, -8, +8};

constexpr std::array<uint64_t, 8> combined_shifts_masks = {
    FILE_A | ROW_ONE,
    FILE_H | ROW_ONE,
    FILE_A | ROW_SEVEN,
    FILE_H | ROW_SEVEN,
    FILE_A,
    FILE_H,
    ROW_ONE,
    ROW_SEVEN,
};

constexpr std::array<int32_t, 8> knight_move_row = {-2, -2, -1, 1,
                                                    +2, +2, +1, -1};
constexpr std::array<int32_t, 8> knight_move_col = {-1, +1, +2, +2,
                                                    +1, -1, -2, -2};
}; // namespace MoveExplorer

#endif // !SEARCH_H
