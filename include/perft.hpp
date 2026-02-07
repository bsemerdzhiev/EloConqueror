#ifndef TREE_SEARCH_H
#define TREE_SEARCH_H

#include "board.hpp"

namespace Perft {
uint64_t search(Board &board, int32_t depth);
}

#endif // !TREE_SEARCH_H
