#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>

struct Move;
struct UndoMove;

enum class SquareType : int8_t {
  KING_WHITE = 0,
  KING_BLACK = 1,
  QUEEN_WHITE = 2,
  QUEEN_BLACK = 3,
  ROOK_WHITE = 4,
  ROOK_BLACK = 5,
  BISHOP_WHITE = 6,
  BISHOP_BLACK = 7,
  KNIGHT_WHITE = 8,
  KNIGHT_BLACK = 9,
  PAWN_WHITE = 10,
  PAWN_BLACK = 11,
  EMPTY = 12,
};

class Board {
public:
  static constexpr int8_t BOARD_ROWS = 8;
  static constexpr int8_t BOARD_COLS = 8;
  static constexpr int8_t ALL_PIECE_TYPES = 6;

  Board();

  Board(std::string fen_string);

  static inline uint64_t getPositionAsBitboard(int8_t row, int8_t col) {
    return (uint64_t{1} << (row * BOARD_COLS + col));
  }
  static inline uint64_t shiftPosition(uint64_t pos, int8_t dir,
                                       uint64_t mask) {
    if (dir < 0) {
      return (pos & (~mask)) >> (-dir);
    } else {
      return (pos & (~mask)) << dir;
    }
  }

  void displayBoard() const;

  static uint64_t chessSquareAsPosition(std::string chess_square);
  static std::string positionAsChessSquare(uint64_t pos);

  void makeMove(const Move &move_to_make, UndoMove &undo_move);

  void unmakeMove(const UndoMove &undo_move);

  void makeMove(const std::string &move_to_make);

  inline bool isCellNotEmpty(uint64_t to_pos, bool turn) const {
    return static_cast<bool>(_all_pieces[turn] & to_pos);
  }

  inline bool isCellNotEmpty(uint64_t to_pos) const {
    return static_cast<bool>((_all_pieces[0] | _all_pieces[1]) & to_pos);
  }

  inline void recomputePiecesPositions() {
    _all_pieces[0] = _pieces[0][0] | _pieces[0][1] | _pieces[0][2] |
                     _pieces[0][3] | _pieces[0][4] | _pieces[0][5];

    _all_pieces[1] = _pieces[1][0] | _pieces[1][1] | _pieces[1][2] |
                     _pieces[1][3] | _pieces[1][4] | _pieces[1][5];
  }

  bool isUnderCheck(uint64_t pos_to_check, bool turn) const;
  bool isEnPassant(uint64_t pos, bool turn) const;
  // 1 - short castle ... 0 - long castle
  bool checkCastlingRights(bool turn, bool castle_type) const;

  uint64_t getPiece(int8_t piece_type, bool turn) const;
  bool getPlayerTurn() const;
  uint64_t getLastMoveTwoSquaresPushPawn() const;

  SquareType getPieceOnSquare(uint64_t sq) const;

private:
  /*
   * elements at ind 0 represent white figures, 1 is for black
   * 0 - king
   * 1 - queen
   * 2 - rook
   * 3 - bishop
   * 4 - knight
   * 5 - pawn
   */
  uint64_t _pieces[2][6];
  uint64_t _all_pieces[2];
  // uint64_t _all_pieces[2];
  /*
   * Set to 0 if last move
   * was not a two square push from a pawn.
   * Set to the pawn's square otherwise
   */
  uint64_t _last_move_two_squares_push_pawn;
  uint64_t _pieces_not_moved;
  bool _player_turn;
};

#endif // !BOARD_H
