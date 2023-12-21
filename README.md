# Compressed Partition Indices (CPI)

## What is This
This repository contains compressed containers designed specifically to store partition ids created during graph partitioning.
Currently, there is only one working container and a small benchmark tool:
- [Run-Length Compressed Representation](include/cpi/run_length_compression.hpp)
- [Benchmark Tool](benchmarks/cpi_benchmark.cpp)

## How to Use
CPI is very easy to use.
We describe how to get CPI below.

```cpp
#include "cpi/run_length_compression.hpp" // include files necessary for the desired compression

// ...

std::vector<uint16_t> partition_ids = { 0, 1, 1, 1 /*...*/}; // A file containing all partition ids
cpi::RunLengthCompression rlc(partition_ids); // Create a compressed version of the partition ids

auto p_id = rlc[42]; // access partition ids;
rlc.print_statistics(); // print statistics
```

## Get CPI
Both, including CPI in your project and using it directly for benchmarks is  rather easy.
The former can be done using CMake and the latter only needs you to clone this repository.
We take care of all dependencies.

### Including CPI in Your Project
CPI is an easy to use C++ project utilizing CMake.
To get this CPI running with your (CMake) project, you can simply include it using `FetchContent_Declare` as follows.

```CMake
include(FetchContent)

FetchContent_Declare(
  cpi
  GIT_REPOSITORY https://github.com/kurpicz/cpi.git
)

FetchContent_MakeAvailable(cpi)
```

Then, you can simply link against the CPI-library:

```CMake
target_link_libraries(<YOUR_LIBRARY/EXECUTABLE>
  PRIVATE cpi
  ... # more libraries to link
)
```

### Building CPI

The first line clones the repository.
The second line enters the root dictionary of the project.
The third and fourth lines compile the project.
If you want to run tests, continue with the fifth and sixth line.

```bash
git clone git@github.com:kurpicz/cpi
cd cpi
cmake --preset release
cmake --build --preset release
cd build
ctest
```
