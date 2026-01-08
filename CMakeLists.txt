cmake_minimum_required(VERSION 3.16)

project(elo-conqueror)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CTest)

add_subdirectory("tests")

add_library(EloConquerorLib "src/board.cpp" "src/search.cpp"
                            "src/tree-search.cpp")

target_include_directories(EloConquerorLib PUBLIC "include/")

add_executable(EloConqueror "src/main.cpp")
target_link_libraries(EloConqueror PRIVATE EloConquerorLib)

add_test(NAME "PERFT" COMMAND tests)
