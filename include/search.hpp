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
}; // namespace MoveExplorer

#endif // !SEARCH_H
