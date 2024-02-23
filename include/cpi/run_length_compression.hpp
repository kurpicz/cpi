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
#include <atomic>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <la_vector.hpp>
#include <pasta/bit_vector/bit_vector.hpp>
#include <vector>

namespace cpi {
/*!
 * \brief Class to run-length compress a vector while maintaining fast access
 * support.
 *
 * \tparam Alphabet Alphabet type of the vector that is run-length compressed.
 */
template <typename Alphabet, std::size_t UncompressedRuns = 64>
class RunLengthCompression {
  //! The character of each run (head).
  std::vector<Alphabet> run_heads_;
  //! The starting position of each run in the (uncompressed) vector.
  std::vector<std::size_t> head_positions_;
  //! Predecessor support for \c head_positions_ to allow fast access.
  la_vector<std::size_t, 6> rank_select_;
  //! Size of the uncompressed vector, i.e., number of uncompressed symbols.
  size_t size_ = 0;

  size_t compressed_elements_;

  std::array<Alphabet, UncompressedRuns> uncompressed_run_heads_ = {0};
  std::array<std::int16_t, UncompressedRuns> uncompressed_run_starts_ = {0};
  size_t uncompressed_runs_;
  size_t uncompressed_elements_;


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
    head_positions_.push_back(0);

    for (std::size_t i = 1; i < input.size(); ++i) {
      if (input[i] != run_heads_.back()) {
        run_heads_.push_back(input[i]);
        head_positions_.push_back(i);
      }
    }
    // head_positions_.push_back(input.size());
    rank_select_ = la_vector<std::size_t, 6>(head_positions_);
    compressed_elements_ = input.size();
  }

  RunLengthCompression() : uncompressed_runs_(0), compressed_elements_(0) {}

  /*!
   * \brief Add characters to the end of the run length compression.
   *
   * \param value Value of the caracter that is appended.
   */
  void push_back(Alphabet const value) {
    ++size_;
    if (uncompressed_runs_ > 0) {
      if (uncompressed_run_heads_[uncompressed_runs_ - 1] == value) {
        ++uncompressed_elements_;
        return;
      }
      if (uncompressed_runs_ < UncompressedRuns) {
        uncompressed_run_starts_[uncompressed_runs_] = uncompressed_elements_++;
        uncompressed_run_heads_[uncompressed_runs_++] = value;
        return;
      }
      compress();
    }
    uncompressed_run_starts_[0] = 0;
    uncompressed_run_heads_[0] = value;
    uncompressed_runs_ = 1;
    uncompressed_elements_ = 1;
  }

  /*!
   * \brief Access any symbol in the compressed vector.
   *
   * \param index Index of the symbol in the (uncompressed) vector.
   * \return Symbol at index \c index in the uncompressed vector.
   */
  [[nodiscard("[RLC] Access operator called but not used.")]] Alphabet
  operator[](std::size_t index) const {
    if (index < compressed_elements_) {
      auto rank = rank_select_.rank(index + 1);
      return run_heads_[rank - 1];
    } else {
      index -= compressed_elements_;
      size_t run_pos = 0;
      while (run_pos < uncompressed_runs_ &&
             uncompressed_run_starts_[run_pos + 1] <= index) {
        ++run_pos;
      }
      return uncompressed_run_heads_[std::min(run_pos, uncompressed_runs_ - 1)];
    }
  }

  /*!
   * \brief Get the size of the uncompressed vector.
   *
   * \return Size of the uncompressed vector.
   */
  [[nodiscard("[RLC] Size computed but not used.")]] std::size_t size() const {
    return size_;
  }

  /*!
   * \brief Print statistics that can be used in SQLPlotTools.
   */
  void print_statistics() const {
    std::size_t const space_in_bytes = size_t{
        run_heads_.size() * sizeof(Alphabet) + rank_select_.size_in_bytes()};
    auto const space_in_mib =
        double{space_in_bytes} / double{1024.0} / double{1024.0};
    std::cout << "space_in_bytes=" << space_in_bytes
              << " uncompressed_space_in_bytes=" << size_ * sizeof(Alphabet)
              << " space_in_mib=" << space_in_mib << " relative="
              << 100.0 / (size_ * sizeof(Alphabet)) * space_in_bytes;
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

private:
  void compress() {
    std::vector<std::size_t> global_run_starts; 
    std::transform(
        uncompressed_run_starts_.begin(),
        uncompressed_run_starts_.end(),
        std::back_inserter(global_run_starts),
        [compressed_elements = compressed_elements_](std::size_t start) {
          return start + compressed_elements;
        });
    rank_select_.append(global_run_starts.begin(),
                        global_run_starts.end());
    run_heads_.insert(run_heads_.end(),
                      uncompressed_run_heads_.begin(),
                      uncompressed_run_heads_.end());
    compressed_elements_ += uncompressed_elements_;
  }

}; // class RunLengthCompression

}; // namespace cpi

/******************************************************************************/
