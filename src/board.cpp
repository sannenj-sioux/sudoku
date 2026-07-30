#include "board.h"

#include <array>
#include <utility>

namespace {
constexpr int kUntrackedValue = -1;

void MarkCellViolation(point_value_t& cell, ConstraintViolation violation) {
  cell.violation |= violation;
}
}  // namespace

Board::Board() {
  reset();
}

Board::Board(const Board& other) : cells_(other.cells_) {
  rebuildBlocks();
  refreshValidationState();
}

Board& Board::operator=(const Board& other) {
  if (this != &other) {
    cells_ = other.cells_;
    rebuildBlocks();
    refreshValidationState();
  }
  return *this;
}

Board::Board(Board&& other) noexcept : cells_(std::move(other.cells_)) {
  rebuildBlocks();
  refreshValidationState();
}

Board& Board::operator=(Board&& other) noexcept {
  if (this != &other) {
    cells_ = std::move(other.cells_);
    rebuildBlocks();
    refreshValidationState();
  }
  return *this;
}

void Board::reset() {
  cells_.fill({static_cast<int>(UNSELECTED), State::INITED, ConstraintViolation::NONE});
  rebuildBlocks();
  refreshValidationState();
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

void Board::refreshValidationState() {
  for (point_value_t& cell : cells_) {
    cell.violation = ConstraintViolation::NONE;
  }

  bool has_row_violation = false;
  bool has_column_violation = false;
  bool has_box_violation = false;

  for (int row = 0; row < GRID_SIZE; ++row) {
    std::array<int, GRID_SIZE + 1> first_index_by_value;
    first_index_by_value.fill(kUntrackedValue);

    for (int column = 0; column < GRID_SIZE; ++column) {
      const auto index =
          (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
          static_cast<std::size_t>(column);
      const int value = cells_.at(index).value;
      if (value < 1 || value > GRID_SIZE) {
        continue;
      }

      int& first_index = first_index_by_value.at(static_cast<std::size_t>(value));
      if (first_index == kUntrackedValue) {
        first_index = static_cast<int>(index);
        continue;
      }

      has_row_violation = true;
      MarkCellViolation(cells_.at(static_cast<std::size_t>(first_index)), ConstraintViolation::ROW);
      MarkCellViolation(cells_.at(index), ConstraintViolation::ROW);
    }
  }

  for (int column = 0; column < GRID_SIZE; ++column) {
    std::array<int, GRID_SIZE + 1> first_index_by_value;
    first_index_by_value.fill(kUntrackedValue);

    for (int row = 0; row < GRID_SIZE; ++row) {
      const auto index =
          (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
          static_cast<std::size_t>(column);
      const int value = cells_.at(index).value;
      if (value < 1 || value > GRID_SIZE) {
        continue;
      }

      int& first_index = first_index_by_value.at(static_cast<std::size_t>(value));
      if (first_index == kUntrackedValue) {
        first_index = static_cast<int>(index);
        continue;
      }

      has_column_violation = true;
      MarkCellViolation(cells_.at(static_cast<std::size_t>(first_index)), ConstraintViolation::COLUMN);
      MarkCellViolation(cells_.at(index), ConstraintViolation::COLUMN);
    }
  }

  for (int box_row = 0; box_row < BOX_SIZE; ++box_row) {
    for (int box_column = 0; box_column < BOX_SIZE; ++box_column) {
      std::array<int, GRID_SIZE + 1> first_index_by_value;
      first_index_by_value.fill(kUntrackedValue);

      const int start_row = box_row * BOX_SIZE;
      const int start_column = box_column * BOX_SIZE;
      for (int row = start_row; row < start_row + BOX_SIZE; ++row) {
        for (int column = start_column; column < start_column + BOX_SIZE; ++column) {
          const auto index =
              (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
              static_cast<std::size_t>(column);
          const int value = cells_.at(index).value;
          if (value < 1 || value > GRID_SIZE) {
            continue;
          }

          int& first_index = first_index_by_value.at(static_cast<std::size_t>(value));
          if (first_index == kUntrackedValue) {
            first_index = static_cast<int>(index);
            continue;
          }

          has_box_violation = true;
          MarkCellViolation(cells_.at(static_cast<std::size_t>(first_index)), ConstraintViolation::BOX);
          MarkCellViolation(cells_.at(index), ConstraintViolation::BOX);
        }
      }
    }
  }

  violation_summary_ = ConstraintViolation::NONE;
  if (has_row_violation) {
    violation_summary_ |= ConstraintViolation::ROW;
  }
  if (has_column_violation) {
    violation_summary_ |= ConstraintViolation::COLUMN;
  }
  if (has_box_violation) {
    violation_summary_ |= ConstraintViolation::BOX;
  }

  validation_state_ = CreateBoardValidationState(violation_summary_);
}

bool Board::isValidState() const {
  return validation_state_->IsValid();
}

ConstraintViolation Board::violationSummary() const {
  return violation_summary_;
}

const BoardValidationState& Board::validationState() const {
  return *validation_state_;
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