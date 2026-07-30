#pragma once

#include <memory>

#include "common.h"

struct CellValueUpdatedEvent {
  bool is_given;
  int value;
  ConstraintViolation violations;
};

class CellValidationState;
class CellGivenState;
class CellErasedState;
class CellUserValueValidState;
class CellUserValueInvalidRowState;
class CellUserValueInvalidColumnState;
class CellUserValueInvalidBoxState;
class CellUserValueInvalidMixedState;

namespace cell_validation_detail {
inline std::unique_ptr<CellValidationState> BuildGivenState();
inline std::unique_ptr<CellValidationState> BuildErasedState();
inline std::unique_ptr<CellValidationState> BuildUserValueState(ConstraintViolation violations);
}  // namespace cell_validation_detail

class CellValidationState {
 public:
  virtual ~CellValidationState() = default;

  virtual std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const = 0;
  virtual bool IsGiven() const = 0;
  virtual bool IsErased() const = 0;
  virtual bool IsUserValue() const = 0;
  virtual ConstraintViolation Violation() const = 0;
  virtual const char* Name() const = 0;
};

class CellGivenState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return true; }
  bool IsErased() const override { return false; }
  bool IsUserValue() const override { return false; }
  ConstraintViolation Violation() const override { return ConstraintViolation::NONE; }
  const char* Name() const override { return "CellGivenState"; }
};

class CellErasedState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return false; }
  bool IsErased() const override { return true; }
  bool IsUserValue() const override { return false; }
  ConstraintViolation Violation() const override { return ConstraintViolation::NONE; }
  const char* Name() const override { return "CellErasedState"; }
};

class CellUserValueValidState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return false; }
  bool IsErased() const override { return false; }
  bool IsUserValue() const override { return true; }
  ConstraintViolation Violation() const override { return ConstraintViolation::NONE; }
  const char* Name() const override { return "CellUserValueValidState"; }
};

class CellUserValueInvalidRowState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return false; }
  bool IsErased() const override { return false; }
  bool IsUserValue() const override { return true; }
  ConstraintViolation Violation() const override { return ConstraintViolation::ROW; }
  const char* Name() const override { return "CellUserValueInvalidRowState"; }
};

class CellUserValueInvalidColumnState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return false; }
  bool IsErased() const override { return false; }
  bool IsUserValue() const override { return true; }
  ConstraintViolation Violation() const override { return ConstraintViolation::COLUMN; }
  const char* Name() const override { return "CellUserValueInvalidColumnState"; }
};

class CellUserValueInvalidBoxState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return false; }
  bool IsErased() const override { return false; }
  bool IsUserValue() const override { return true; }
  ConstraintViolation Violation() const override { return ConstraintViolation::BOX; }
  const char* Name() const override { return "CellUserValueInvalidBoxState"; }
};

class CellUserValueInvalidMixedState : public CellValidationState {
 public:
  std::unique_ptr<CellValidationState> OnCellValueUpdated(
      const CellValueUpdatedEvent& event) const override {
    if (event.is_given) {
      return cell_validation_detail::BuildGivenState();
    }
    if (event.value == static_cast<int>(UNSELECTED)) {
      return cell_validation_detail::BuildErasedState();
    }
    return cell_validation_detail::BuildUserValueState(event.violations);
  }

  bool IsGiven() const override { return false; }
  bool IsErased() const override { return false; }
  bool IsUserValue() const override { return true; }
  ConstraintViolation Violation() const override {
    return ConstraintViolation::ROW | ConstraintViolation::COLUMN | ConstraintViolation::BOX;
  }
  const char* Name() const override { return "CellUserValueInvalidMixedState"; }
};

namespace cell_validation_detail {
inline std::unique_ptr<CellValidationState> BuildGivenState() {
  return std::make_unique<CellGivenState>();
}

inline std::unique_ptr<CellValidationState> BuildErasedState() {
  return std::make_unique<CellErasedState>();
}

inline std::unique_ptr<CellValidationState> BuildUserValueState(ConstraintViolation violations) {
  const bool has_row = HasViolation(violations, ConstraintViolation::ROW);
  const bool has_column = HasViolation(violations, ConstraintViolation::COLUMN);
  const bool has_box = HasViolation(violations, ConstraintViolation::BOX);
  const int active_violation_count = static_cast<int>(has_row) + static_cast<int>(has_column) +
                                     static_cast<int>(has_box);

  if (active_violation_count == 0) {
    return std::make_unique<CellUserValueValidState>();
  }
  if (active_violation_count > 1) {
    return std::make_unique<CellUserValueInvalidMixedState>();
  }
  if (has_row) {
    return std::make_unique<CellUserValueInvalidRowState>();
  }
  if (has_column) {
    return std::make_unique<CellUserValueInvalidColumnState>();
  }
  return std::make_unique<CellUserValueInvalidBoxState>();
}
}  // namespace cell_validation_detail

class CellValidationStateMachine {
 public:
  CellValidationStateMachine() : state_(cell_validation_detail::BuildErasedState()) {}

  void OnCellValueUpdated(bool is_given, int value, ConstraintViolation violations) {
    const CellValueUpdatedEvent event{is_given, value, violations};
    state_ = state_->OnCellValueUpdated(event);
  }

  bool IsGiven() const { return state_->IsGiven(); }
  bool IsErased() const { return state_->IsErased(); }
  bool IsUserValue() const { return state_->IsUserValue(); }
  ConstraintViolation Violation() const { return state_->Violation(); }
  const char* StateName() const { return state_->Name(); }

 private:
  std::unique_ptr<CellValidationState> state_;
};
