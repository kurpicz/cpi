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
/*!
 * \brief Class to run-length compress a vector while maintaining fast access support.
 *
 * \tparam Alphabet Alphabet type of the vector that is run-length compressed.
 */
template <typename Alphabet>
class RunLengthCompression {
  //! The character of each run (head).
  std::vector<Alphabet> run_heads_;
  //! The starting position of each run in the (uncompressed) vector.
  std::vector<std::size_t> head_positions_;
  //! Predecessor support for \c head_positions_ to allow fast access.
  la_vector<std::size_t, 6> rank_select_;
  //! Size of the uncompressed vector, i.e., number of uncompressed symbols.
  size_t size_ = 0;
  //! Largest symbol in the vector (used for statistics only).
  Alphabet max_symbol_ = 0;

public:
  /*!
   * \brief Constructor computing the run-length compression.
   *
   * \param input Vector that is run-length compressed.
   */
  RunLengthCompression(std::vector<Alphabet> const& input)
      : size_(input.size()) {
    if (input.size() == 0) {
      return;
    }

    run_heads_.push_back(input.front());
    run_heads_.push_back(input.front());
    head_positions_.push_back(0);
    max_symbol_ = run_heads_.back();

    for (std::size_t i = 1; i < input.size(); ++i) {
      if (input[i] != run_heads_.back()) {
        run_heads_.push_back(input[i]);
        head_positions_.push_back(i);
        max_symbol_ = std::max(max_symbol_, run_heads_.back());
      }
    }
    //head_positions_.push_back(input.size());
    rank_select_ = la_vector<std::size_t, 6>(head_positions_);
  }

  /*!
   * \brief Add characters to the end of the run length compression.
   *
   * \param value Value of the caracter that is appended.
   */
  void push_back(Alphabet const value) {
    if (value != run_heads_.back()) {
      max_symbol_ = std::max(max_symbol_, value);
      run_heads_.push_back(value);
      rank_select_.append(size_++);
    } else {
      ++size_;
    }
  }

  /*!
   * \brief Access any symbol in the compressed vector.
   *
   * \param index Index of the symbol in the (uncompressed) vector.
   * \return Symbol at index \c index in the uncompressed vector.
   */
  [[nodiscard("[RLC] Access operator called but not used.")]]
  Alphabet operator[](std::size_t index) const {
    std::size_t const rank = rank_select_.rank(index + 1);
    if (rank >= run_heads_.size()) {
      return run_heads_.back();
    }
    return run_heads_[rank];
  }

  /*!
   * \brief Get the size of the uncompressed vector.
   *
   * \return Size of the uncompressed vector.
   */
  [[nodiscard("[RLC] Size computed but not used.")]]
  std::size_t size() const {
    return size_;
  }

  /*!
   * \brief Print statistics that can be used in SQLPlotTools.
   */
  void print_statistics() const {
    std::size_t const space_in_bytes = size_t{run_heads_.size() * sizeof(Alphabet) +
                                   rank_select_.size_in_bytes()};
    auto const space_in_mib = double{space_in_bytes} / double{1024.0} / double{1024.0};
    std::cout << "space_in_bytes=" << space_in_bytes
	      << " uncompressed_space_in_bytes=" << size_ * sizeof(Alphabet)
	      << " space_in_mib=" << space_in_mib 
              << " relative=" << 100.0 / (size_ * sizeof(Alphabet)) * space_in_bytes;
  }

  /*!
   * \brief Print statistics about the runs.
   *
   * Prints "<char that run consists of> : <starting position>" for each run.
   */
  void print_runs() const {
    for (size_t i = 0; i + 1 < run_heads_.size(); ++i) {
      std::cout << run_heads_[i + 1] << ": " << head_positions_[i] << "\n";

    }
  }

}; // class RunLengthCompression

}; // namespace cpi

/******************************************************************************/
