#include "uci.hpp"
#include "alpha-beta.hpp"
#include "board.hpp"
#include "move.hpp"
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
    {"uci", 0}, {"isready", 1}, {"position", 2}, {"go", 3}, {"d", 4},
};

static const std::unordered_map<std::string, int32_t> position_table = {
    {"startpos", 0},
    {"fen", 1},
};

static const std::unordered_map<std::string, int32_t> go_table = {
    {"searchmoves", 0},
    {"depth", 1},
    {"perft", 2},
};

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
        break;
      }
      case 1: {
        std::getline(iss, fen);
        game_board = Board{fen.substr(1, fen.length() - 1)};

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

        Move move = AlphaBeta::searchMove(game_board, depth);
        std::cout << std::format("bestmove {}\n", move.formatted());
        break;
      }
      case 2: {
        int32_t perft_depth;
        iss >> perft_depth;

        Perft::search(game_board, perft_depth);
        break;
      }
      }

      break;
    }
    case 4: {
      game_board.displayBoard();
      break;
    }
    }
  }
}
