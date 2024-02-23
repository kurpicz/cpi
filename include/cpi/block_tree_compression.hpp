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
#include <iterator>
#include <pasta/block_tree/block_tree.hpp>
#include <pasta/block_tree/construction/block_tree_fp.hpp>
#include <sstream>
#include <vector>

namespace cpi {

template <typename Alphabet>
class BlockTreeCompression {
  size_t size_;
  size_t max_partition_id_;

  pasta::BlockTreeFP<Alphabet, int64_t>* bt_;

public:
  BlockTreeCompression(std::vector<Alphabet>& input) : size_(input.size()) {
    bt_ = pasta::make_block_tree_fp<Alphabet, int64_t>(input, 8, 16);
  }

  Alphabet operator[](size_t index) const {
    return bt_[index];
  }

  void print_statistics() const {
    std::cout << "bt->print_space_usage() " << bt_->print_space_usage() << '\n';
  }

private:
}; // class BlockTreeCompression

}; // namespace cpi

/******************************************************************************/
