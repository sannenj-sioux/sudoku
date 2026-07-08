#pragma once

#include <array>
#include <cstddef>

#include "common.h"

class Board {
 public:
  Board();

  void reset();

  point_value_t& at(std::size_t index);
  const point_value_t& at(std::size_t index) const;

  std::size_t size() const;

 private:
  std::array<point_value_t, CELL_COUNT> cells_;
};