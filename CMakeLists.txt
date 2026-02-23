cmake_minimum_required(VERSION 3.16)

project(elo-conqueror)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_NATIVE "Enable native CPU optimizations" ON)
option(ENABLE_LTO "Enable LTO optimizations" ON)

include(CTest)

add_subdirectory("tests")

add_library(
  EloConquerorLib "src/board.cpp" "src/move-generator.cpp" "src/perft.cpp"
                  "src/evaluate.cpp" "src/alpha-beta.cpp" "src/uci.cpp")

target_include_directories(EloConquerorLib PUBLIC "include/")

add_executable(EloConqueror "src/main.cpp")
target_link_libraries(EloConqueror PRIVATE EloConquerorLib)

target_compile_options(EloConquerorLib PRIVATE -O3 -DNDEBUG)
target_compile_options(EloConqueror PRIVATE -O3 -DNDEBUG)

if(ENABLE_NATIVE)
  target_compile_options(EloConquerorLib PRIVATE -march=native -mtune=native)
  target_compile_options(EloConqueror PRIVATE -march=native -mtune=native)
endif()

if(ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
  if(ipo_supported)
    set_property(TARGET EloConquerorLib PROPERTY INTERPROCEDURAL_OPTIMIZATION
                                                 TRUE)
    set_property(TARGET EloConqueror PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "IPO/LTO not supported: ${ipo_error}")
  endif()
endif()

add_test(NAME "PERFT" COMMAND perft-tests)
add_test(NAME "EVAL" COMMAND eval-tests)
