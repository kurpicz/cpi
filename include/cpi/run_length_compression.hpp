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

#include "pasta/bit_vector/bit_vector.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

namespace cpi {

template <typename Alphabet>
class RunLengthCompression {
  std::vector<Alphabet> run_heads_;
  std::vector<size_t> head_positions_;
  // RankStructure ranks_;

  size_t size_ = 0;
  size_t max_partition_id_ = 0;

public:
  RunLengthCompression(std::vector<Alphabet> const& input)
      : size_(input.size()) {
    if (input.size() == 0) {
      return;
    }
    run_heads_.push_back(input.front());
    head_positions_.push_back(0);
    max_partition_id_ = run_heads_.back();

    head_positions_.push_back(0);

    for (size_t i = 1; i < input.size(); ++i) {
      if (input[i] != run_heads_.back()) {
        run_heads_.push_back(input[i]);
        head_positions_.push_back(i);
        max_partition_id_ =
            std::max<size_t>(max_partition_id_, run_heads_.back());
      }
    }
    // ranks_ = RankStructure(head_positions_);
  }

  void push_back(Alphabet&& value) {
    if (value != run_heads_.back()) {
      run_heads_.push_back(value);
      head_positions_.push_back(size_++);
    } else {
      ++size_;
    }
  }

  Alphabet operator[](size_t index) const {
    // size_t const run_pos = ranks_.rank(index);
    // return run_heads_[run_pos];
    return Alphabet{index};
  }

  void print_statistics() const {
    std::cout << "nodes=" << size_ << " runs=" << run_heads_.size()
              << " avg_run_length=" << 1.0f * size_ / run_heads_.size() << "\n";

    std::cout << "space_usage=" << run_heads_.size() + size_ / 8.0 << '\n';
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
