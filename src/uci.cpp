#include "uci.hpp"
#include "alpha-beta.hpp"
#include "board.hpp"
#include "perft.hpp"

#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace UCI {
Board game_board;
}

const std::string ENGINE_NAME = "EloConqueror 0.1";

static const std::unordered_map<std::string, int32_t> command_table = {
    {"uci", 0}, {"isready", 1}, {"position", 2},
    {"go", 3},  {"d", 4},       {"ucinewgame", 5},
};

static const std::unordered_map<std::string, int32_t> position_table = {
    {"startpos", 0},
    {"fen", 1},
};

static const std::unordered_map<std::string, int32_t> go_table = {
    {"searchmoves", 0},
    {"depth", 1},
    {"perft", 2},
    {"wtime", 3},
};

void parseMoves(std::istringstream &iss) {
  std::string command;
  if (iss >> command) {
    std::string new_move;
    while (iss >> new_move) {
      UCI::game_board.makeMove(new_move);
    }
  }
}

void UCI::run() {
  std::string line;
  std::string command;
  std::string fen;

  while (true) {
    std::getline(std::cin, line);

    std::istringstream iss(line);

    iss >> command;

    switch (command_table.at(command)) {
    case 0: {
      std::cout << std::format("id name {}\n", ENGINE_NAME);
      std::cout << std::format("id author {}\n", ENGINE_NAME);
      std::cout << "uciok\n";
      break;
    }
    case 1: {
      std::cout << "readyok\n";
      break;
    }
    case 2: {
      iss >> command;
      switch (position_table.at(command)) {
      case 0: {
        game_board = Board{};

        // TODO: read moves
        parseMoves(iss);
        break;
      }
      case 1: {
        std::getline(iss, fen);
        game_board = Board{fen.substr(1, fen.length() - 1)};

        parseMoves(iss);
        // TODO: read moves
        break;
      }
      }
      break;
    }
    case 3: {
      iss >> command;

      switch (go_table.at(command)) {
      case 0: {
        break;
      }
      case 1: {
        int32_t depth;
        iss >> depth;

        AlphaBeta::searchMove(game_board, depth);

        break;
      }
      case 2: {
        int32_t perft_depth;
        iss >> perft_depth;

        Perft::search(game_board, perft_depth);
        break;
      }
      case 3: {
        int32_t time_w, time_b, rem_moves, depth;
        std::string b_time, rem_moves_str, depth_str;

        iss >> time_w >> b_time >> time_b >> rem_moves_str >> rem_moves >>
            depth_str >> depth;

        AlphaBeta::searchMove(game_board, depth);
        break;
      }
      }

      break;
    }
    case 4: {
      game_board.displayBoard();
      break;
    }
    case 5: {
      game_board = Board{};
    }
    }
  }
}
