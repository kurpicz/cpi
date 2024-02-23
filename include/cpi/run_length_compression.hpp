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
#include <iterator>

namespace cpi {
/*!
 * \brief Class to run-length compress a vector while maintaining fast access support.
 *
 * \tparam Alphabet Alphabet type of the vector that is run-length compressed.
 */
template <typename Alphabet, std::size_t UncompressedBufferSize = 10>
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

  size_t compressed_elements_;

  std::array<Alphabet, UncompressedBufferSize> buffer_ = { 0 };
  size_t buffered_elements_;

  std::vector<size_t> all_head_positions_;
  
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
    head_positions_.push_back(1);
    all_head_positions_.push_back(0);
    max_symbol_ = run_heads_.back();

    for (std::size_t i = 1; i < input.size(); ++i) {
      if (input[i] != run_heads_.back()) {
        run_heads_.push_back(input[i]);
        head_positions_.push_back(i);
        all_head_positions_.push_back(i);
        max_symbol_ = std::max(max_symbol_, run_heads_.back());
      }
    }
    //head_positions_.push_back(input.size());
    rank_select_ = la_vector<std::size_t, 6>(head_positions_);
    compressed_elements_ = input.size();
  }

  RunLengthCompression() :  buffered_elements_(0), compressed_elements_(0) { }

  /*!
   * \brief Add characters to the end of the run length compression.
   *
   * \param value Value of the caracter that is appended.
   */
  void push_back(Alphabet const value) {
    buffer_[buffered_elements_++] = value;
    if (buffered_elements_ == buffer_.size()) {
      std::cout << "compressing " << '\n';
      for (size_t i = 0; i < buffer_.size(); ++i) {
        std::cout << "buffer_[i] " << buffer_[i] << '\n';
      }
      compress(buffer_.begin(), buffer_.end());
      compressed_elements_ += buffered_elements_;
      buffered_elements_ = 0;
    }
    ++size_;
  }

  void print_all_head_positions() const {
    for (size_t i = 0; i < all_head_positions_.size(); ++i) {
      std::cout << "all_head_positions_[" << i << "] " << all_head_positions_[i] << '\n';
    }
  }

  auto all_head_positions() {
    return all_head_positions_;
  }

  auto rank(size_t index) const {
    return rank_select_.rank(index);
  }
  
  /*!
   * \brief Access any symbol in the compressed vector.
   *
   * \param index Index of the symbol in the (uncompressed) vector.
   * \return Symbol at index \c index in the uncompressed vector.
   */
  [[nodiscard("[RLC] Access operator called but not used.")]] Alphabet
  operator[](std::size_t index) const {
    // for (auto const& head : run_heads_) {
    //   std::cout << "head " << head << ", ";
    // }
    // std::cout << '\n';
    // std::cout << "index " << index << '\n';
    // std::cout << "compressed_elements_ " << compressed_elements_ << '\n';
    if (index < compressed_elements_) {
      auto rank = rank_select_.rank(index + 1);
      // std::cout << "rank " << rank << '\n';
      // std::cout << "run_heads_.size() " << run_heads_.size() << '\n';
      if (rank == run_heads_.size()) {
        --rank;
      }
      // if (rank < 1) {
      //   ++rank;
      // }
      return run_heads_[rank];
    } else {
      // std::cout << "buffer_[index - compressed_elements_] " << buffer_[index - compressed_elements_] << '\n';
      return buffer_[index - compressed_elements_];
    }
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

private:
  void compress(std::random_access_iterator auto const cbegin,
                std::random_access_iterator auto const end) {
    auto begin = cbegin;
    std::vector<std::size_t> local_run_starts;
    if (!run_heads_.empty()) {
      // run_heads_.pop_back();
      // std::cout << "*begin " << *begin << '\n';
      while (begin != end && *begin == run_heads_.back()) {
        ++begin;
      }
      // std::cout << "std::distance(begin, cbegin) " << std::distance(begin, cbegin) << '\n';
    } else {
      // run_heads_.push_back(*begin);
    }
    while(begin < end) {
      run_heads_.push_back(*begin);
      std::cout << "run_heads_.back() " << run_heads_.back() << '\n';
      if (auto start = std::distance(cbegin, begin); start != 0 || compressed_elements_ != 0)
        local_run_starts.push_back(start);

      if (!local_run_starts.empty())
        std::cout << "local_run_starts.back() " << local_run_starts.back() << '\n';
      ++begin;
      while (begin < end && *begin == run_heads_.back()) {
        ++begin;
      }
    }
    if (local_run_starts.size() > 0) {
      std::transform(
                     local_run_starts.begin(),
                     local_run_starts.end(),
                     local_run_starts.begin(),
                     [compressed_elements = compressed_elements_](std::size_t start) {
                       return start + compressed_elements;
                     });
      for (auto const& run_start : local_run_starts) {
        std::cout << "run_start " << run_start << '\n';
      }
      for (auto const& rh : run_heads_) {
        std::cout << "rh " << rh << '\n';
      }
      rank_select_.append(local_run_starts.begin(), local_run_starts.end());
    }
    compressed_elements_ += std::distance(begin, end);
    for (size_t i = 0; i < compressed_elements_; ++i) {
      std::cout << "rank_select_.rank(" << i << ") " << rank_select_.rank(i) << '\n';
    }
    // run_heads_.push_back(*std::prev(end));
  }

}; // class RunLengthCompression

}; // namespace cpi

/******************************************************************************/
