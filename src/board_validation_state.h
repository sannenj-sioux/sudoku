#pragma once

#include <cstddef>

#include "common.h"

enum class BoardValidationKind {
  VALID,
  INVALID_ROW,
  INVALID_COLUMN,
  INVALID_BOX,
  INVALID_MIXED,
  COUNT,
};

class BoardValidationState final {
 public:
  explicit BoardValidationState(BoardValidationKind kind = BoardValidationKind::VALID)
      : kind_(kind) {}

  bool IsValid() const { return kind_ == BoardValidationKind::VALID; }

  const char* Name() const {
    static constexpr const char* kNames[] = {
        "BoardValidState",
        "BoardInvalidRowState",
        "BoardInvalidColumnState",
        "BoardInvalidBoxState",
        "BoardInvalidMixedState",
    };
    static_assert((sizeof(kNames) / sizeof(kNames[0])) == kKindCount,
                  "BoardValidationKind and name lookup table are out of sync.");

    return kNames[static_cast<std::size_t>(kind_)];
  }

  const char* Message() const {
    static constexpr const char* kMessages[] = {
        "Board is valid",
        "Row constraint violated",
        "Column constraint violated",
        "Box constraint violated",
        "Multiple constraints violated",
    };
    static_assert((sizeof(kMessages) / sizeof(kMessages[0])) == kKindCount,
                  "BoardValidationKind and message lookup table are out of sync.");

    return kMessages[static_cast<std::size_t>(kind_)];
  }

 private:
  static constexpr std::size_t kKindCount =
      static_cast<std::size_t>(BoardValidationKind::COUNT);
  BoardValidationKind kind_;
};

inline BoardValidationState CreateBoardValidationState(ConstraintViolation violation_summary) {
  const bool has_row = HasViolation(violation_summary, ConstraintViolation::ROW);
  const bool has_column = HasViolation(violation_summary, ConstraintViolation::COLUMN);
  const bool has_box = HasViolation(violation_summary, ConstraintViolation::BOX);

  const int active_violation_count = static_cast<int>(has_row) + static_cast<int>(has_column) +
                                     static_cast<int>(has_box);

  if (active_violation_count == 0) {
    return BoardValidationState(BoardValidationKind::VALID);
  }
  if (active_violation_count > 1) {
    return BoardValidationState(BoardValidationKind::INVALID_MIXED);
  }
  if (has_row) {
    return BoardValidationState(BoardValidationKind::INVALID_ROW);
  }
  if (has_column) {
    return BoardValidationState(BoardValidationKind::INVALID_COLUMN);
  }
  return BoardValidationState(BoardValidationKind::INVALID_BOX);
}
