#ifndef SEARCH_H
#define SEARCH_H

#include "board.hpp"
#include "move.hpp"

#include <vector>

namespace MoveExplorer {
std::vector<Move> searchAllMoves(Board, bool, bool);
std::vector<Move> searchKingMoves(Board, bool, bool);
std::vector<Move> searchQueenMoves(Board, bool, bool);
std::vector<Move> searchRookMoves(Board, bool, bool);
std::vector<Move> searchBishopMoves(Board, bool, bool);
std::vector<Move> searchKnightMoves(Board, bool, bool);
std::vector<Move> searchPawnMoves(Board, bool, bool);

const int32_t move_row_diag[4] = {-1, -1, +1, +1};
const int32_t move_col_diag[4] = {-1, +1, -1, +1};

const int32_t move_row_line[4] = {0, 0, -1, +1};
const int32_t move_col_line[4] = {-1, +1, 0, 0};
}; // namespace MoveExplorer

#endif // !SEARCH_H
