#ifndef SEARCH_H
#define SEARCH_H

#include "board.hpp"
#include "move.hpp"

#include <array>
#include <vector>

namespace MoveGenerator {
void searchAllMoves(Board &board, const bool turn, std::vector<Move> &moves);
void searchKingMoves(Board &board, const bool turn, std::vector<Move> &moves);
void searchQueenMoves(Board &board, const bool turn, std::vector<Move> &moves);
void searchRookMoves(Board &board, const bool turn, std::vector<Move> &moves);
void searchBishopMoves(Board &board, const bool turn, std::vector<Move> &moves);
void searchKnightMoves(Board &board, const bool turn, std::vector<Move> &moves);
void searchPawnMoves(Board &board, const bool turn, std::vector<Move> &moves);

//-------------------------------------------------------------------------------------------------------------------------

extern uint64_t KING_ATTACK_SQUARES[64];
extern uint64_t PAWN_ATTACK_SQUARES[64][2];
extern uint64_t KNIGHT_ATTACK_SQUARES[64];
extern uint64_t DIAG_ATTACK_SQUARES[64][4];
extern uint64_t LINE_ATTACK_SQUARES[64][4];

void initAttackTables();

//-------------------------------------------------------------------------------------------------------------------------

constexpr uint64_t FILE_A = 0x0101010101010101ULL;
constexpr uint64_t FILE_B = 0x0202020202020202ULL;
constexpr uint64_t FILE_AB = FILE_A | FILE_B;

constexpr uint64_t FILE_G = 0x4040404040404040ULL;
constexpr uint64_t FILE_H = 0x8080808080808080ULL;
constexpr uint64_t FILE_GH = FILE_G | FILE_H;

constexpr uint64_t ROW_ONE = 0x00000000000000FFULL;
constexpr uint64_t ROW_TWO = 0x000000000000FF00ULL;
constexpr uint64_t ROW_ONE_TWO = ROW_ONE | ROW_TWO;

constexpr uint64_t ROW_SIX = 0x00FF000000000000ULL;
constexpr uint64_t ROW_SEVEN = 0xFF00000000000000ULL;
constexpr uint64_t ROW_SIX_SEVEN = ROW_SIX | ROW_SEVEN;

//-------------------------------------------------------------------------------------------------------------------------

constexpr uint64_t rook_from[2][2] = {
    {Board::getPositionAsBitboard(0, 0), Board::getPositionAsBitboard(0, 7)},
    {Board::getPositionAsBitboard(7, 0), Board::getPositionAsBitboard(7, 7)}};

const uint64_t rook_to[2][2] = {
    {Board::getPositionAsBitboard(0, 3), Board::getPositionAsBitboard(0, 5)},
    {Board::getPositionAsBitboard(7, 3), Board::getPositionAsBitboard(7, 5)}};

//-------------------------------------------------------------------------------------------------------------------------

//    3   4
//      X
//    1   2
//
constexpr std::array<int8_t, 4> move_diag_shifts = {-9, -7, +7, +9};
constexpr std::array<uint64_t, 4> move_diag_shifts_masks = {
    FILE_A | ROW_ONE, FILE_H | ROW_ONE, FILE_A | ROW_SEVEN, FILE_H | ROW_SEVEN};

//-------------------------------------------------------------------------------------------------------------------------

//       4
//     1 X 2
//       3
//
constexpr std::array<int8_t, 4> move_line_shifts = {-1, +1, -8, +8};
constexpr std::array<uint64_t, 4> move_line_shifts_masks = {FILE_A, FILE_H,
                                                            ROW_ONE, ROW_SEVEN};

//-------------------------------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------------------------------

constexpr std::array<int8_t, 8> knight_move_shifts = {
    -17, -15, -6, +10, +17, +15, +6, -10,
};

constexpr std::array<uint64_t, 8> knight_move_shifts_masks = {
    ROW_ONE_TWO | FILE_A, ROW_ONE_TWO | FILE_H,   ROW_ONE | FILE_GH,
    ROW_SEVEN | FILE_GH,  ROW_SIX_SEVEN | FILE_H, ROW_SIX_SEVEN | FILE_A,
    ROW_SEVEN | FILE_AB,  ROW_ONE | FILE_AB};
}; // namespace MoveGenerator

#endif // !SEARCH_H
