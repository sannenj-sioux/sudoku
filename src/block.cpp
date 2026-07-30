#include "block.h"

#include <cassert>
#include <iostream>
#include <string>

#include "color.h"
#include "common.h"
#include "display_symbol.h"

namespace {
Color::Code ResolveViolationColor(ConstraintViolation violation) {
  const bool row = HasViolation(violation, ConstraintViolation::ROW);
  const bool column = HasViolation(violation, ConstraintViolation::COLUMN);
  const bool box = HasViolation(violation, ConstraintViolation::BOX);
  const int active_count = static_cast<int>(row) + static_cast<int>(column) + static_cast<int>(box);

  if (active_count > 1) {
    return Color::FG_WHITE;
  }
  if (row) {
    return Color::FG_LIGHT_RED;
  }
  if (column) {
    return Color::FG_LIGHT_CYAN;
  }
  if (box) {
    return Color::FG_LIGHT_MAGENTA;
  }
  return Color::FG_DEFAULT;
}
}  // namespace

CBlock::CBlock() : _count(0) {}

bool CBlock::isValid() const {
  assert(MAX_COUNT == _count);

  for (int i = 0; i < _count - 1; ++i) {
    for (int j = i + 1; j < _count; ++j) {
      if (UNSELECTED == _numbers[i]->value || UNSELECTED == _numbers[j]->value) {
        continue;
      }

      if (_numbers[i]->value == _numbers[j]->value) {
        return false;
      }
    }
  }

  return true;
}

bool CBlock::isFull() const {
  for (int i = 0; i < _count; ++i) {
    point_value_t* p_point_value = _numbers[i];
    if (nullptr == p_point_value || UNSELECTED == p_point_value->value) {
      return false;
    }
  }
  return true;
}

void CBlock::print(int cur_point, int highlighted_num) const {
  std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << PIPE
            << Color::Modifier() << " ";
  for (int i = 0; i < _count; ++i) {
    auto number = *(_numbers[i]);
    Color::Code pipe_color;
    Color::Code num_bgcolor;
    Color::Code num_fgcolor;
    std::string num = " ";

    if ((i + 1) % 3 == 0) {
      pipe_color = Color::FG_RED;
    } else {
      pipe_color = Color::FG_DEFAULT;
    }

    if (i == cur_point) {
      num_bgcolor = Color::BG_BLUE;
    } else if (highlighted_num != UNSELECTED && number.value == highlighted_num) {
      num_bgcolor =
          Color::BG_YELLOW;  // Highlight numbers matching the current cursor value in yellow
    } else {
      num_bgcolor = Color::BG_DEFAULT;
    }

    if (number.state == State::ERASED) {
      num_fgcolor = Color::FG_GREEN;
    } else {
      num_fgcolor = Color::FG_DEFAULT;
    }

    if (number.state == State::ERASED && number.violation != ConstraintViolation::NONE) {
      num_fgcolor = ResolveViolationColor(number.violation);
      int active_violation_count = 0;
      if (HasViolation(number.violation, ConstraintViolation::ROW)) {
        ++active_violation_count;
      }
      if (HasViolation(number.violation, ConstraintViolation::COLUMN)) {
        ++active_violation_count;
      }
      if (HasViolation(number.violation, ConstraintViolation::BOX)) {
        ++active_violation_count;
      }
      const bool is_multi_violation = active_violation_count > 1;
      if (is_multi_violation && num_bgcolor == Color::BG_DEFAULT) {
        num_bgcolor = Color::BG_RED;
      }
    }

    if (number.value != 0) {
      num = std::to_string(number.value);
    }

    std::cout << Color::Modifier(Color::BOLD, num_bgcolor, num_fgcolor) << num << Color::Modifier()
              << " " << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, pipe_color) << PIPE
              << Color::Modifier() << " ";
  }
  std::cout << std::endl;
}

void CBlock::push_back(point_value_t* point) {
  assert(nullptr != point);
  _numbers[_count++] = point;
}

int CBlock::getNumberValue(int index) const {
  if (index >= 0 && index < _count) {
    return _numbers[index]->value;
  }
  return UNSELECTED;  // Or throw an exception
}