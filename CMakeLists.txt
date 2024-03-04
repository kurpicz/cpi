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
  la_vector
  GIT_REPOSITORY https://github.com/kurpicz/la_vector.git 
  GIT_TAG        4b92552 #main
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

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.0 # or a later release
)

FetchContent_MakeAvailable(tlx la_vector pasta_bit_vector pasta_block_tree Catch2)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cpi INTERFACE)
target_include_directories(cpi
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(cpi
  INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/include/cpi/run_length_compression.hpp)
target_link_libraries(cpi
  INTERFACE la_vector
  INTERFACE pasta_bit_vector)



add_executable(cpi_benchmark
  benchmarks/cpi_benchmark.cpp)
target_include_directories(cpi_benchmark
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks)
target_link_libraries(cpi_benchmark
  PRIVATE cpi
  PRIVATE pasta_block_tree
  PRIVATE tlx)

enable_testing()
add_executable(run_length_compression_test
  tests/run_length_compression_test.cpp)
target_link_libraries(run_length_compression_test
  PRIVATE Catch2::Catch2WithMain
  PRIVATE cpi)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(run_length_compression_test)
