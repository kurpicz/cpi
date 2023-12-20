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

#pragma once

#include <algorithm>
#include <iostream>
#include <la_vector.hpp>
#include <pasta/bit_vector/bit_vector.hpp>
#include <vector>

namespace cpi {

template <typename Alphabet>
class RunLengthCompression {
  std::vector<Alphabet> run_heads_;
  std::vector<std::size_t> head_positions_;
  la_vector<std::size_t, 6> rank_select_;
  size_t size_ = 0;
  size_t max_partition_id_ = 0;

public:
  RunLengthCompression(std::vector<Alphabet> const& input)
      : size_(input.size()) {
    if (input.size() == 0) {
      return;
    }

    run_heads_.push_back(input.front());
    run_heads_.push_back(input.front());
    head_positions_.push_back(0);
    max_partition_id_ = run_heads_.back();

    for (std::size_t i = 1; i < input.size(); ++i) {
      if (input[i] != run_heads_.back()) {
        run_heads_.push_back(input[i]);
        head_positions_.push_back(i);
        max_partition_id_ =
	  std::max<std::size_t>(max_partition_id_, run_heads_.back());
      }
    }
    head_positions_.push_back(input.size());
    rank_select_ = la_vector<std::size_t, 6>(head_positions_);
  }

  void push_back(Alphabet&& value) {
    if (value != run_heads_.back()) {
      run_heads_.push_back(value);
      head_positions_.push_back(size_++);
    } else {
      ++size_;
    }
  }

  Alphabet operator[](std::size_t index) const {
    std::size_t const rank = rank_select_.rank(index + 1);
    return run_heads_[rank];
  }

  std::size_t size() const {
    return size_;
  }

  void print_statistics() const {
    std::size_t const space_in_bytes = size_t{run_heads_.size() * sizeof(Alphabet) +
                                   rank_select_.size_in_bytes()};
    auto const space_in_mib = double{space_in_bytes} / double{1024.0} / double{1024.0};
    std::cout << "space_in_bytes=" << space_in_bytes
	      << " uncompressed_space_in_bytes=" << size_ * sizeof(Alphabet)
	      << " space_in_mib=" << space_in_mib 
              << " relative=" << 100.0 / (size_ * sizeof(Alphabet)) * space_in_bytes;
  }

  void print_runs() const {
    for (size_t i = 0; i + 1 < run_heads_.size(); ++i) {
      std::cout << run_heads_[i + 1] << ": " << head_positions_[i] << "\n";

    }
  }

private:
  std::vector<size_t> compute_run_head_histogram() const {
    std::vector<size_t> hist(max_partition_id_ + 1, 0);
    for (auto const head : run_heads_) {
      ++hist[head];
    }
    return hist;
  }

}; // class RunLengthCompression

}; // namespace cpi

/******************************************************************************/
