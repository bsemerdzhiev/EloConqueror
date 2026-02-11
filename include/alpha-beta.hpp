#ifndef ALPHA_BETA_H
#define ALPHA_BETA_H

#include <cstdint>

struct Move;
class Board;

namespace AlphaBeta {
void searchMove(Board &board, int32_t depth);
};

#endif // !ALPHA_BETA_H
