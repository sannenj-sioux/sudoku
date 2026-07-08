#include "puzzle_generator.h"

#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "common.h"
#include "utility.inl"

namespace {

std::size_t ToIndex(int row, int column) {
  return (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
         static_cast<std::size_t>(column);
}

void SetCell(Board& board, int row, int column, int value, State state) {
  point_value_t& cell = board.at(ToIndex(row, column));
  cell.value = value;
  cell.state = state;
}

}  // namespace

Board PuzzleGenerator::GenerateSolvedBoard() {
  std::vector<std::vector<int>> matrix(static_cast<std::size_t>(GRID_SIZE),
                                       std::vector<int>(static_cast<std::size_t>(GRID_SIZE), 0));

  for (int num = 0; num < BOX_SIZE; ++num) {
    std::vector<int> unit = ShuffleUnit();
    const int start_index = num * BOX_SIZE;
    for (int row = start_index; row < start_index + BOX_SIZE; ++row) {
      for (int column = start_index; column < start_index + BOX_SIZE; ++column) {
        matrix[row][column] = unit.back();
        unit.pop_back();
      }
    }
  }

  std::vector<std::tuple<int, int>> box_list;
  box_list.reserve(static_cast<std::size_t>(CELL_COUNT));
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      if (matrix[row][column] == 0) {
        box_list.emplace_back(row, column);
      }
    }
  }

  std::map<std::string, std::vector<int>> available_num{};
  int full_num = 0;
  const int empty_num = static_cast<int>(box_list.size());
  while (full_num < empty_num) {
    const std::tuple<int, int> position = box_list[full_num];
    const int row = std::get<0>(position);
    const int column = std::get<1>(position);
    std::vector<int> able_unit;
    const std::string key = std::to_string(row) + "x" + std::to_string(column);
    if (available_num.find(key) == available_num.end()) {
      able_unit = GetUnit();
      for (int box_row = row / BOX_SIZE * BOX_SIZE; box_row < row / BOX_SIZE * BOX_SIZE + BOX_SIZE;
           ++box_row) {
        for (int box_column = column / BOX_SIZE * BOX_SIZE;
             box_column < column / BOX_SIZE * BOX_SIZE + BOX_SIZE; ++box_column) {
          able_unit.erase(
              std::remove(able_unit.begin(), able_unit.end(), matrix[box_row][box_column]),
              able_unit.end());
        }
      }
      for (int current_column = 0; current_column < GRID_SIZE; ++current_column) {
        if (matrix[row][current_column] != 0) {
          able_unit.erase(
              std::remove(able_unit.begin(), able_unit.end(), matrix[row][current_column]),
              able_unit.end());
        }
      }
      for (int current_row = 0; current_row < GRID_SIZE; ++current_row) {
        if (matrix[current_row][column] != 0) {
          able_unit.erase(
              std::remove(able_unit.begin(), able_unit.end(), matrix[current_row][column]),
              able_unit.end());
        }
      }
      available_num[key] = able_unit;
    } else {
      able_unit = available_num[key];
    }

    if (available_num[key].empty()) {
      full_num -= 1;
      if (available_num.find(key) != available_num.end()) {
        available_num.erase(key);
      }
      matrix[row][column] = 0;
      continue;
    }

    matrix[row][column] = available_num[key].back();
    available_num[key].pop_back();
    full_num += 1;
  }

  Board board;
  board.reset();
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      SetCell(board, row, column, matrix[row][column], State::INITED);
    }
  }

  return board;
}

void PuzzleGenerator::EraseCells(Board& board, int count) {
  std::vector<int> cell_indexes(CELL_COUNT);
  for (int i = 0; i < CELL_COUNT; ++i) {
    cell_indexes[i] = i;
  }

  for (int i = 0; i < count; ++i) {
    const auto random_index =
        static_cast<std::size_t>(RandomInt(0, static_cast<int>(cell_indexes.size() - 1)));
    board.at(static_cast<std::size_t>(cell_indexes.at(random_index))) =
        {static_cast<int>(UNSELECTED), State::ERASED};
    cell_indexes.erase(cell_indexes.begin() +
                       static_cast<std::vector<int>::difference_type>(random_index));
  }
}