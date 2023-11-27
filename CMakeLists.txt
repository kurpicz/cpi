cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

project(compressed_partition_indices)

FetchContent_Declare(
  tlx
  GIT_REPOSITORY https://github.com/tlx/tlx.git
  GIT_TAG        b6af589 #release 0.6.1
)

FetchContent_Declare(
  pasta_bit_vector
  GIT_REPOSITORY https://github.com/pasta-toolbox/bit_vector.git
  GIT_TAG        b4798d5 #main
)

FetchContent_Declare(
  pasta_block_tree
  GIT_REPOSITORY https://github.com/pasta-toolbox/block_tree.git
  GIT_TAG        8ca8744 #main
)

FetchContent_MakeAvailable(tlx pasta_bit_vector pasta_block_tree)


add_executable(cpi_benchmark
  benchmarks/cpi_benchmark.cpp)
target_include_directories(cpi_benchmark
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cpi_benchmark
  PRIVATE pasta_bit_vector
  PRIVATE pasta_block_tree
  PRIVATE tlx)
