#pragma once

[[maybe_unused]] static const unsigned int UNSELECTED = 0;

enum class Difficulty : int { EASY = 1, NORMAL, HARD };

enum class State : int {
  INITED = 0,
  ERASED,
};

struct point_t {
  int x;
  int y;
};

struct point_value_t {
  int value;
  State state;
};

class CPointSort {
 public:
  bool operator()(const point_t& lhs, const point_t& rhs) const {
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
  }
};

