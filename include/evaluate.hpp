#pragma once

#include <cstdint>

class Board;

using EvaluationScoreType = int32_t;

namespace Evaluate {
void initTables();
EvaluationScoreType evaluateBoard(const Board &board);
}; // namespace Evaluate
