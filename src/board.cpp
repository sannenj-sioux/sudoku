#include "board.h"

Board::Board() {
  reset();
}

void Board::reset() {
  cells_.fill({static_cast<int>(UNSELECTED), State::INITED});
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