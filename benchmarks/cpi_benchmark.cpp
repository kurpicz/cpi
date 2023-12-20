/*******************************************************************************
 * This file is part of cli
 *
 * Copyright (C) 2023 Florian Kurpicz <florian@kurpicz.org>
 *
 * cli is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cli is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cli.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "benchmark_file_reader.hpp"
#include "cpi/run_length_compression.hpp"

#include <filesystem>
#include <tlx/cmdline_parser.hpp>

class CLIBenchmark {
  std::filesystem::path path_;

public:
  CLIBenchmark(std::filesystem::path path) : path_(path) {}

  void run() {
    cpi::BenchmarkFileReader<uint16_t> file_reader(path_);
    file_reader.read_file();

    cpi::RunLengthCompression rlc(file_reader.partition_ids());


    std::cout << "RESULT"
              << " algorithm=rlc"
              << " input=" << path_.filename().string() << " ";

    rlc.print_statistics();

    std::cout << "\n";

  }

}; // class CLIBenchmark

int32_t main(int32_t argc, char const* const argv[]) {
  tlx::CmdlineParser cp;

  cp.set_description(
      "Benchmark tool for representations of compressed partition indices.");
  cp.set_author("Florian Kurpicz <florian@kurpicz.org>");

  std::string input_path = "";
  cp.add_param_string("input", input_path, "Path to input file.");

  if (!cp.process(argc, argv)) {
    return -1;
  }

  CLIBenchmark benchmark(input_path);
  benchmark.run();

  return 0;
}

/******************************************************************************/
