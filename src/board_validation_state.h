#pragma once

#include <memory>

#include "common.h"

class BoardValidationState {
 public:
  virtual ~BoardValidationState() = default;

  virtual bool IsValid() const = 0;
  virtual const char* Name() const = 0;
  virtual const char* Message() const = 0;
};

class BoardValidState : public BoardValidationState {
 public:
  bool IsValid() const override { return true; }
  const char* Name() const override { return "BoardValidState"; }
  const char* Message() const override { return "Board is valid"; }
};

class BoardInvalidRowState : public BoardValidationState {
 public:
  bool IsValid() const override { return false; }
  const char* Name() const override { return "BoardInvalidRowState"; }
  const char* Message() const override { return "Row constraint violated"; }
};

class BoardInvalidColumnState : public BoardValidationState {
 public:
  bool IsValid() const override { return false; }
  const char* Name() const override { return "BoardInvalidColumnState"; }
  const char* Message() const override { return "Column constraint violated"; }
};

class BoardInvalidBoxState : public BoardValidationState {
 public:
  bool IsValid() const override { return false; }
  const char* Name() const override { return "BoardInvalidBoxState"; }
  const char* Message() const override { return "Box constraint violated"; }
};

class BoardInvalidMixedState : public BoardValidationState {
 public:
  bool IsValid() const override { return false; }
  const char* Name() const override { return "BoardInvalidMixedState"; }
  const char* Message() const override { return "Multiple constraints violated"; }
};

inline std::unique_ptr<BoardValidationState> CreateBoardValidationState(
    ConstraintViolation violation_summary) {
  const bool has_row = HasViolation(violation_summary, ConstraintViolation::ROW);
  const bool has_column = HasViolation(violation_summary, ConstraintViolation::COLUMN);
  const bool has_box = HasViolation(violation_summary, ConstraintViolation::BOX);

  const int active_violation_count = static_cast<int>(has_row) + static_cast<int>(has_column) +
                                     static_cast<int>(has_box);

  if (active_violation_count == 0) {
    return std::make_unique<BoardValidState>();
  }
  if (active_violation_count > 1) {
    return std::make_unique<BoardInvalidMixedState>();
  }
  if (has_row) {
    return std::make_unique<BoardInvalidRowState>();
  }
  if (has_column) {
    return std::make_unique<BoardInvalidColumnState>();
  }
  return std::make_unique<BoardInvalidBoxState>();
}
