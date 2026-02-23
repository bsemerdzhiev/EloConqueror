include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(perft-tests "perf-test.cpp")
add_executable(eval-tests "eval-test.cpp")
target_link_libraries(perft-tests PRIVATE Catch2::Catch2WithMain
                                          EloConquerorLib)
target_link_libraries(eval-tests PRIVATE Catch2::Catch2WithMain EloConquerorLib)
