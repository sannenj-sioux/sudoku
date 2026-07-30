#include "board.h"

#include <utility>

Board::Board() {
  reset();
}

Board::Board(const Board& other) : cells_(other.cells_) {
  rebuildBlocks();
}

Board& Board::operator=(const Board& other) {
  if (this != &other) {
    cells_ = other.cells_;
    rebuildBlocks();
  }
  return *this;
}

Board::Board(Board&& other) noexcept : cells_(std::move(other.cells_)) {
  rebuildBlocks();
}

Board& Board::operator=(Board&& other) noexcept {
  if (this != &other) {
    cells_ = std::move(other.cells_);
    rebuildBlocks();
  }
  return *this;
}

void Board::reset() {
  cells_.fill({static_cast<int>(UNSELECTED), State::INITED});
  rebuildBlocks();
}

point_value_t& Board::at(std::size_t index) {
  return cells_.at(index);
}

const point_value_t& Board::at(std::size_t index) const {
  return cells_.at(index);
}

std::size_t Board::size() const {
  return cells_.size();
}

const std::array<CBlock, GRID_SIZE>& Board::rowBlocks() const {
  return row_blocks_;
}

const std::array<CBlock, GRID_SIZE>& Board::columnBlocks() const {
  return column_blocks_;
}

const std::array<std::array<CBlock, BOX_SIZE>, BOX_SIZE>& Board::boxBlocks() const {
  return box_blocks_;
}

void Board::rebuildBlocks() {
  for (int column = 0; column < GRID_SIZE; ++column) {
    CBlock column_block;
    for (int row = 0; row < GRID_SIZE; ++row) {
      const auto index =
          (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
          static_cast<std::size_t>(column);
      column_block.push_back(&(cells_.at(index)));
    }
    column_blocks_.at(static_cast<std::size_t>(column)) = column_block;
  }

  for (int row = 0; row < GRID_SIZE; ++row) {
    CBlock row_block;
    for (int column = 0; column < GRID_SIZE; ++column) {
      const auto index =
          (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
          static_cast<std::size_t>(column);
      row_block.push_back(&(cells_.at(index)));
    }
    row_blocks_.at(static_cast<std::size_t>(row)) = row_block;
  }

  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      CBlock& box_block = box_blocks_.at(static_cast<std::size_t>(row / BOX_SIZE))
                              .at(static_cast<std::size_t>(column / BOX_SIZE));
      if ((row % BOX_SIZE) == 0 && (column % BOX_SIZE) == 0) {
        box_block = CBlock();
      }
      const auto index =
          (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
          static_cast<std::size_t>(column);
      box_block.push_back(&(cells_.at(index)));
    }
  }
}