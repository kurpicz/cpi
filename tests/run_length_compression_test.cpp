/*******************************************************************************
 * This file is part of cpi
 *
 * Copyright (C) 2023 Florian Kurpicz <florian@kurpicz.org>
 *
 * cpi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cpi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cpi.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <random>

#include <cpi/run_length_compression.hpp>

std::vector<std::uint32_t> generate_runs(std::size_t const total_length,
                                         std::size_t const max_run_length) {
  std::vector<uint32_t> result;
  result.reserve(total_length);

  // Seed with a real random value, if available
  std::random_device random_device;
 
  // Choose a random mean between 1 and 6
  std::default_random_engine random_engine(random_device());
  std::uniform_int_distribution<std::size_t> run_length(1, max_run_length);
  std::uniform_int_distribution<std::uint32_t> character(0, std::numeric_limits<std::uint32_t>::max());
    
  std::size_t inserted_characters = 0;
  while (inserted_characters < total_length) {
    std::size_t const cur_run_length =
      std::min(run_length(random_engine),
	       total_length - inserted_characters);
    std::uint32_t c = character(random_engine);
    for (std::size_t i = 0; i < cur_run_length; ++i) {
      result.push_back(c);
    }
    inserted_characters += cur_run_length;
  }
  return result;
}

// TEST_CASE("Run-length compression static construction", "[RUN-LENGTH COMPRESSION]") {
//   auto max_run_length = GENERATE(1, 2, 4, 8, 16, 32, 128, 256);
//   auto input = generate_runs(1'000'000, max_run_length);
//   cpi::RunLengthCompression rlc(input);

//   REQUIRE(input.size() == rlc.size());

//   for (size_t i = 0; i < input.size(); ++i) {
//     CHECK(input[i] == rlc[i]);
//   }
// }

TEST_CASE("Run-length compression appending", "[RUN-LENGTH COMPRESSION]") {
  auto max_run_length = GENERATE(32);//, 128, 256);
  auto input = generate_runs(1'000, 2/*max_run_length*/);
  cpi::RunLengthCompression<decltype(input)::value_type> rlc;

  auto to_append = generate_runs(40, max_run_length);

  for (size_t i = 0; i < to_append.size(); ++i) {
    std::cout << "(size_t)to_append[i] " << (size_t)to_append[i] << ' ' << i <<  '\n';
  }

  for (auto i = size_t{0}; i < to_append.size(); ++i) {
    // std::cout << "i " << i << " / " << to_append.size() << " appending " << to_append[i] << '\n';
    rlc.push_back(to_append[i]);
  }

  size_t i = 0;
  // for (; i < input.size(); ++i) {
  //   // CHECK(input[i] == rlc[i]);
  //   if (input[i] != rlc[i])
  //     std::cout << "i " << i << '\n';
  // }
  for (; i < to_append.size(); ++i) {
    // std::cout << "asking for i " << i << '\n';
    CHECK(to_append[i] == rlc[i]);
    if (to_append[i] != rlc[i]) {
      std::cout << "###i " << i << '\n';
      // std::cout << "to_append[i - input.size()] " << to_append[i - input.size()] << " vs " << rlc[i] << '\n';
    }
  }
  // rlc.print_all_head_positions();

  // for (auto const hp : rlc.all_head_positions()) {
  //   std::cout << hp << " | " << rlc[hp] << " & " << rlc[hp + 1] << std::endl;
  //   std::cout << rlc.rank(hp) << " & " << rlc.rank(hp + 1) << std::endl;
  // }
  
}

/******************************************************************************/
