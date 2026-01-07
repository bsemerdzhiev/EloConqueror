#ifndef SEARCH_H
#define SEARCH_H

#include "board.hpp"
#include "move.hpp"

#include <array>
#include <vector>

namespace MoveExplorer {
std::vector<Move> searchAllMoves(const Board &board, const bool turn,
                                 const bool should_move);
std::vector<Move> searchKingMoves(const Board &board, const bool turn,
                                  const bool should_move);
std::vector<Move> searchQueenMoves(const Board &board, const bool turn,
                                   const bool should_move);
std::vector<Move> searchRookMoves(const Board &board, const bool turn,
                                  const bool should_move);
std::vector<Move> searchBishopMoves(const Board &board, const bool turn,
                                    const bool should_move);
std::vector<Move> searchKnightMoves(const Board &board, const bool turn,
                                    const bool should_move);
std::vector<Move> searchPawnMoves(const Board &board, const bool turn,
                                  const bool should_move);

constexpr std::array<int32_t, 4> move_row_diag = {-1, -1, +1, +1};
constexpr std::array<int32_t, 4> move_col_diag = {-1, +1, -1, +1};

constexpr std::array<int32_t, 4> move_row_line = {0, 0, -1, +1};
constexpr std::array<int32_t, 4> move_col_line = {-1, +1, 0, 0};
}; // namespace MoveExplorer

#endif // !SEARCH_H
