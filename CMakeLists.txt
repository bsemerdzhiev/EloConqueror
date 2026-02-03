cmake_minimum_required(VERSION 3.16)

project(elo-conqueror)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_NATIVE "Enable native CPU optimizations" ON)

include(CTest)

add_subdirectory("tests")

add_library(EloConquerorLib "src/board.cpp" "src/search.cpp"
                            "src/tree-search.cpp")

target_include_directories(EloConquerorLib PUBLIC "include/")

add_executable(EloConqueror "src/main.cpp")
target_link_libraries(EloConqueror PRIVATE EloConquerorLib)

if(ENABLE_NATIVE)
  target_compile_options(EloConquerorLib PRIVATE -march=native -mtune=native)
  target_compile_options(EloConqueror PRIVATE -march=native -mtune=native)
endif()

add_test(NAME "PERFT" COMMAND tests)
