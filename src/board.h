#pragma once

#include <array>
#include <cstddef>

#include "block.h"
#include "board_validation_state.h"
#include "cell_validation_state_machine.h"
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

  bool isCellGiven(std::size_t index) const;
  bool isCellErased(std::size_t index) const;
  bool isCellUserValue(std::size_t index) const;

  void refreshValidationState();
  bool isValidState() const;
  ConstraintViolation violationSummary() const;
  const BoardValidationState& validationState() const;

 private:
  void rebuildBlocks();

  std::array<point_value_t, CELL_COUNT> cells_;
  std::array<CellValidationStateMachine, CELL_COUNT> cell_state_machines_;
  std::array<CBlock, GRID_SIZE> row_blocks_;
  std::array<CBlock, GRID_SIZE> column_blocks_;
  std::array<std::array<CBlock, BOX_SIZE>, BOX_SIZE> box_blocks_;
  ConstraintViolation violation_summary_;
  BoardValidationState validation_state_;
};