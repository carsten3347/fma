cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

project(fma-test CXX)

include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
  )

# add_compile_options(-Wall)
# add_compile_options(-Werror)

# For x64 to use FMA
# add_compile_options(-march=haswell)

# values: off/on/fast
# add_compile_options(-ffp-contract=on)

set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(benchmark)

add_executable(bench-fma
  src/bench_fma.cpp)

target_link_libraries(bench-fma
  benchmark::benchmark
  )
