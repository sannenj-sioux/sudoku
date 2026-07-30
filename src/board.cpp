#include "board.h"

#include <array>
#include <utility>

namespace {
void MarkCellViolation(point_value_t& cell, ConstraintViolation violation) {
  cell.violation |= violation;
}

bool IsFilledCellForValidation(const point_value_t& cell) {
  return cell.value >= 1 && cell.value <= GRID_SIZE;
}

int RowOf(std::size_t index) {
  return static_cast<int>(index / static_cast<std::size_t>(GRID_SIZE));
}

int ColumnOf(std::size_t index) {
  return static_cast<int>(index % static_cast<std::size_t>(GRID_SIZE));
}

bool HasRowDuplicate(const std::array<point_value_t, CELL_COUNT>& cells,
                     std::size_t index,
                     int value) {
  const int row = RowOf(index);
  for (int column = 0; column < GRID_SIZE; ++column) {
    const auto candidate_index =
        (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
        static_cast<std::size_t>(column);
    if (candidate_index == index) {
      continue;
    }

    const point_value_t& candidate = cells.at(candidate_index);
    if (IsFilledCellForValidation(candidate) && candidate.value == value) {
      return true;
    }
  }
  return false;
}

bool HasColumnDuplicate(const std::array<point_value_t, CELL_COUNT>& cells,
                        std::size_t index,
                        int value) {
  const int column = ColumnOf(index);
  for (int row = 0; row < GRID_SIZE; ++row) {
    const auto candidate_index =
        (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
        static_cast<std::size_t>(column);
    if (candidate_index == index) {
      continue;
    }

    const point_value_t& candidate = cells.at(candidate_index);
    if (IsFilledCellForValidation(candidate) && candidate.value == value) {
      return true;
    }
  }
  return false;
}

bool HasBoxDuplicate(const std::array<point_value_t, CELL_COUNT>& cells,
                     std::size_t index,
                     int value) {
  const int row = RowOf(index);
  const int column = ColumnOf(index);
  const int start_row = (row / BOX_SIZE) * BOX_SIZE;
  const int start_column = (column / BOX_SIZE) * BOX_SIZE;

  for (int box_row = start_row; box_row < start_row + BOX_SIZE; ++box_row) {
    for (int box_column = start_column; box_column < start_column + BOX_SIZE; ++box_column) {
      const auto candidate_index =
          (static_cast<std::size_t>(box_row) * static_cast<std::size_t>(GRID_SIZE)) +
          static_cast<std::size_t>(box_column);
      if (candidate_index == index) {
        continue;
      }

      const point_value_t& candidate = cells.at(candidate_index);
      if (IsFilledCellForValidation(candidate) && candidate.value == value) {
        return true;
      }
    }
  }

  return false;
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
  cells_.fill({static_cast<int>(UNSELECTED), true, ConstraintViolation::NONE});
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

bool Board::isCellGiven(std::size_t index) const {
  return cell_state_machines_.at(index).IsGiven();
}

bool Board::isCellErased(std::size_t index) const {
  return cell_state_machines_.at(index).IsErased();
}

bool Board::isCellUserValue(std::size_t index) const {
  return cell_state_machines_.at(index).IsUserValue();
}

void Board::refreshValidationState() {
  std::array<ConstraintViolation, CELL_COUNT> user_violation_masks{};
  user_violation_masks.fill(ConstraintViolation::NONE);

  for (std::size_t index = 0; index < cells_.size(); ++index) {
    point_value_t& cell = cells_.at(index);
    cell_state_machines_.at(index).OnCellValueUpdated(cell.is_given, cell.value,
                                                      ConstraintViolation::NONE);
    cell.is_given = cell_state_machines_.at(index).IsGiven();
    cell.violation = cell_state_machines_.at(index).Violation();
  }

  bool has_row_violation = false;
  bool has_column_violation = false;
  bool has_box_violation = false;

  for (std::size_t index = 0; index < cells_.size(); ++index) {
    const point_value_t& cell = cells_.at(index);
    if (!cell_state_machines_.at(index).IsUserValue() || !IsFilledCellForValidation(cell)) {
      continue;
    }

    ConstraintViolation mask = ConstraintViolation::NONE;
    if (HasRowDuplicate(cells_, index, cell.value)) {
      mask |= ConstraintViolation::ROW;
      has_row_violation = true;
    }
    if (HasColumnDuplicate(cells_, index, cell.value)) {
      mask |= ConstraintViolation::COLUMN;
      has_column_violation = true;
    }
    if (HasBoxDuplicate(cells_, index, cell.value)) {
      mask |= ConstraintViolation::BOX;
      has_box_violation = true;
    }

    user_violation_masks.at(index) = mask;
  }

  for (std::size_t index = 0; index < cells_.size(); ++index) {
    point_value_t& cell = cells_.at(index);
    cell_state_machines_.at(index).OnCellValueUpdated(cell.is_given, cell.value,
                                                      user_violation_masks.at(index));
    cell.is_given = cell_state_machines_.at(index).IsGiven();
    cell.violation = cell_state_machines_.at(index).Violation();
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
  return validation_state_.IsValid();
}

ConstraintViolation Board::violationSummary() const {
  return violation_summary_;
}

const BoardValidationState& Board::validationState() const {
  return validation_state_;
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