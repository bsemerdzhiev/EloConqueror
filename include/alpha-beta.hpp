#pragma once

#include <cstdint>

struct Move;
class Board;

using ScoreType = int32_t;

namespace AlphaBeta {
void iterativeDeepening(Board &board, std::size_t depth);
};
