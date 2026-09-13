#pragma once

#include "evaluate.hpp"
#include "move.hpp"
#include "zobrist-hash.hpp"

#include <vector>

enum class NodeType {
  Exact,
  UpperBound,
  LowerBound,
};

struct TranspositionTableEntry {
  Move best_move;
  ZobristHash::ZobristHashValue zoobrist_hash;
  std::size_t depth;

  EvaluationScoreType score;
  NodeType node_type;
};

namespace TranspositionTable {
//                                      1M entries
constexpr std::size_t TT_TABLE_SIZE = 1024 * 1024 * 1024;

extern std::vector<TranspositionTableEntry> tt_table_;
}; // namespace TranspositionTable
