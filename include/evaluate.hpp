#ifndef EVALUATE_H
#define EVALUATE_H

#include <cstdint>

class Board;

namespace Evaluate {
void initTables();
int32_t evaluateBoard(const Board &board);
}; // namespace Evaluate

#endif // !EVALUATE_H
