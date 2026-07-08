#pragma once

#include <array>
#include <cstddef>

#include "block.h"
#include "common.h"

class Board {
 public:
  Board();
  Board(const Board& other);
  Board& operator=(const Board& other);
  Board(Board&& other) noexcept;
  Board& operator=(Board&& other) noexcept;

  void reset();

  point_value_t& at(std::size_t index);
  const point_value_t& at(std::size_t index) const;

  std::size_t size() const;

  const std::array<CBlock, GRID_SIZE>& rowBlocks() const;
  const std::array<CBlock, GRID_SIZE>& columnBlocks() const;
  const std::array<std::array<CBlock, BOX_SIZE>, BOX_SIZE>& boxBlocks() const;

 private:
  void rebuildBlocks();

  std::array<point_value_t, CELL_COUNT> cells_;
  std::array<CBlock, GRID_SIZE> row_blocks_;
  std::array<CBlock, GRID_SIZE> column_blocks_;
  std::array<std::array<CBlock, BOX_SIZE>, BOX_SIZE> box_blocks_;
};