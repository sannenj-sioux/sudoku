#pragma once

static constexpr int BOX_SIZE = 3;
static constexpr int GRID_SIZE = BOX_SIZE * BOX_SIZE; // Square grids
[[maybe_unused]] static constexpr int CELL_COUNT = GRID_SIZE * GRID_SIZE;
[[maybe_unused]] static constexpr unsigned int UNSELECTED = 0;

enum class Difficulty : int { EASY = 1, NORMAL, HARD };

enum class State : int {
  INITED = 0,
  ERASED,
};

enum class ConstraintViolation : unsigned char {
  NONE = 0,
  ROW = 1,
  COLUMN = 2,
  BOX = 4,
};

inline ConstraintViolation operator|(ConstraintViolation lhs, ConstraintViolation rhs) {
  return static_cast<ConstraintViolation>(static_cast<unsigned char>(lhs) |
                                          static_cast<unsigned char>(rhs));
}

inline ConstraintViolation& operator|=(ConstraintViolation& lhs, ConstraintViolation rhs) {
  lhs = lhs | rhs;
  return lhs;
}

inline bool HasViolation(ConstraintViolation mask, ConstraintViolation flag) {
  return (static_cast<unsigned char>(mask) & static_cast<unsigned char>(flag)) != 0U;
}

struct point_t {
  int x;
  int y;
};

struct point_value_t {
  int value;
  State state;
  ConstraintViolation violation;
};

class CPointSort {
 public:
  bool operator()(const point_t& lhs, const point_t& rhs) const {
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
  }
};

