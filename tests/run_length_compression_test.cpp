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

TEST_CASE("Run-length compression static construction", "[RUN-LENGTH COMPRESSION]") {
  auto max_run_length = GENERATE(1, 2, 4, 8, 16, 32, 128, 256);
  auto input = generate_runs(1'000'000, max_run_length);
  cpi::RunLengthCompression rlc(input);

  REQUIRE(input.size() == rlc.size());

  for (size_t i = 0; i < input.size(); ++i) {
    CHECK(input[i] == rlc[i]);
  }
}

/******************************************************************************/
