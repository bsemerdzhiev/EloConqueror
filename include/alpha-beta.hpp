#pragma once

#include <cstdint>

struct Move;
class Board;

namespace AlphaBeta {
void iterativeDeepening(Board &board, int32_t depth);
};
