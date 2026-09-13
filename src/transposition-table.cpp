#include "transposition-table.hpp"
#include "evaluate.hpp"
#include "zobrist-hash.hpp"
#include <vector>

std::vector<TranspositionTableEntry>
    TranspositionTable::tt_table_(TranspositionTable::TT_TABLE_SIZE);

const TranspositionTableEntry *TranspositionTable::find(const Board &board) {
  const ZobristHash::ZobristHashValue hash_for_board =
      ZobristHash::hashBoard(board);

  const std::size_t ind = hash_for_board % TranspositionTable::TT_TABLE_SIZE;

  const TranspositionTableEntry &el_at_ind = TranspositionTable::tt_table_[ind];

  if (el_at_ind.zobrist_hash == hash_for_board) {
    return &el_at_ind;
  }
  return nullptr;
}

const TranspositionTableEntry *
TranspositionTable::find(ZobristHash::ZobristHashValue hash_for_board) {
  const std::size_t ind = hash_for_board % TranspositionTable::TT_TABLE_SIZE;

  const TranspositionTableEntry &el_at_ind = TranspositionTable::tt_table_[ind];

  if (el_at_ind.zobrist_hash == hash_for_board) {
    return &el_at_ind;
  }
  return nullptr;
}

void TranspositionTable::insert(const Board &board, const Move &move,
                                const std::size_t depth,
                                const EvaluationScoreType score,
                                const NodeType node_type) {
  const ZobristHash::ZobristHashValue hash_for_board =
      ZobristHash::hashBoard(board);

  const std::size_t ind = hash_for_board % TranspositionTable::TT_TABLE_SIZE;

  TranspositionTable::tt_table_[ind] =
      TranspositionTableEntry{.best_move = move,
                              .zobrist_hash = hash_for_board,
                              .depth = depth,
                              .score = score,
                              .node_type = node_type};
}
