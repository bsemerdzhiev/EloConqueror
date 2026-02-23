# EloConqueror

EloConqueror is a custom C++ chess engine capable of generating legal moves, evaluating board positions, and computing best moves via search.  
This is a personal project focused on core chess engine architecture, performance-oriented design, and extensibility.

[Chess programming](https://www.chessprogramming.org/Main_Page) website is used as a reference over the development of the project.

The engine is under active development and serves as a platform for experimenting with search algorithms, evaluation strategies, and low-level C++ optimizations. Perf is actively used as a guidance in optimization and better low-level design.

---

## Overview

EloConqueror implements the fundamental components of a chess engine:

- Board representation and move generation  
- Alpha-beta search with depth control  
- Static evaluation of positions  
- Best-move selection and CLI output  
- Perft MNPS(Million Nodes Per Second) currently achievable - 40MNPS

While not yet competitive with top engines, the project already provides a complete end-to-end pipeline and is designed to evolve toward more advanced techniques.

---

## Features

- Legal move generation 
- Alpha-beta search with quiescence  
- Position evaluation - currently done with [PeSTO](https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function)
- [Perft](https://www.chessprogramming.org/Perft) testing for move generator validation  
- Command-line interface - [UCI](https://official-stockfish.github.io/docs/stockfish-wiki/UCI-&-Commands.html) standard 
- Modular C++ architecture for future extensions  

---

## Project Structure

Project layout:
```text
.
├── include/ # Header files
├── src/ # Engine implementation
├── tests/ # Unit tests
├── CMakeLists.txt # Build configuration
└── .github/ # CI workflows
```

---

## Getting Started

### Requirements

- C++23-compatible compiler (GCC / Clang / MSVC)
- CMake (>= 3.16)
- Make or Ninja

### Build

```bash
git clone https://github.com/bsemerdzhiev/EloConqueror.git
cd EloConqueror

cmake -S . -B ./build -DENABLE_NATIVE=ON -DENABLE_LTO=ON -G=Ninja 
cmake --build ./build

cd build
./EloConqueror

### To run the tests
cd build
ctest --verbose


