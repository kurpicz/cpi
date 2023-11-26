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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace cpi {

template <typename SizeType>
class BenchmarkFileReader {
  std::filesystem::path path_;

  size_t nodes_ = size_t{0};
  std::vector<SizeType> partition_ids_;

public:
  BenchmarkFileReader(std::filesystem::path path) : path_(path) {}

  size_t nodes() const {
    return nodes_;
  }

  std::vector<SizeType> partition_ids() const {
    return partition_ids_;
  }

  void read_file() {
    std::ifstream file_stream;
    file_stream.open(path_, std::ios_base::in);

    if (file_stream) {
      std::string cur_line;
      // read first line, which gives us the number of nodes in the graph
      if (!file_stream.eof()) {
        std::getline(file_stream, cur_line);
        nodes_ = std::stoi(cur_line);
      }
      partition_ids_.resize(nodes_);

      size_t cur_node = 0;
      // read partition id of nodes
      while (!file_stream.eof() && cur_node < nodes_) {
        std::getline(file_stream, cur_line);
        partition_ids_[cur_node++] = std::stoi(cur_line);
      }

      // TODO read queries

      file_stream.close();
    } else {
      std::cerr << "File could not be opened" << '\n';
    }
  }
};

}; // namespace cpi

/******************************************************************************/
