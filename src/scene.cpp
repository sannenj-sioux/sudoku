#include "scene.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "color.h"
#include "common.h"
#include "display_symbol.h"
#include "i18n.h"
#include "utility.inl"

namespace {
constexpr char KEY_ESC = 0x1B;
constexpr char KEY_UNDO = 0x75;
constexpr char KEY_ENTER = 0x0D;
constexpr char KEY_UP = 0x48;
constexpr char KEY_LEFT = 0x4B;
constexpr char KEY_DOWN = 0x50;
constexpr char KEY_RIGHT = 0x4D;
}  // namespace

CScene::CScene(int index) : _max_column(static_cast<int>(pow(index, 2))), _cur_point({0, 0}) {
  init();
}

CScene::~CScene() = default;

void CScene::show() const {
  ClearScreen();

  printUnderline();

  // Get the number at the cursor position (if cursor is within bounds)
  int highlighted_num = UNSELECTED;
  if (_cur_point.y >= 0 && _cur_point.y < _max_column) {
    const CBlock& cursor_block = _row_block.at(static_cast<size_t>(_cur_point.y));
    highlighted_num = cursor_block.getNumberValue(_cur_point.x);
  }

  for (int row = 0; row < _max_column; ++row) {
    const CBlock& block = _row_block.at(static_cast<size_t>(row));
    if (_cur_point.y == row) {
      block.print(_cur_point.x, highlighted_num);
    } else {
      block.print(-1, highlighted_num);
    }
    printUnderline(row);
  }
}

void CScene::printUnderline(int line_no) const {
  auto is_curline = (_cur_point.y == line_no);
  for (int column = 0; column < _max_column; ++column) {
    if ((column % BOX_SIZE) == 0 || line_no == -1 || ((line_no + 1) % BOX_SIZE) == 0) {
      std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << CORNER
                << Color::Modifier();
    } else {
      std::cout << CORNER;
    }
    const char* third_symbol = (is_curline && _cur_point.x == column) ? ARROW : LINE;
    if (line_no == -1 || ((line_no + 1) % BOX_SIZE) == 0) {
      std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << LINE
                << third_symbol << LINE << Color::Modifier();
    } else {
      std::cout << LINE << third_symbol << LINE;
    }
  }
  std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << CORNER
            << Color::Modifier() << '\n';
}

void CScene::init() {
  _map.fill({static_cast<int>(UNSELECTED), State::INITED});

  for (int col = 0; col < _max_column; ++col) {
    CBlock column_block;

    for (int row = 0; row < _max_column; ++row) {
      const auto index =
          (static_cast<size_t>(row) * static_cast<size_t>(_max_column)) + static_cast<size_t>(col);
      column_block.push_back(&(_map.at(index)));
    }

    _column_block.at(static_cast<size_t>(col)) = column_block;
  }

  for (int row = 0; row < _max_column; ++row) {
    CBlock row_block;

    for (int col = 0; col < _max_column; ++col) {
      const auto index =
          (static_cast<size_t>(row) * static_cast<size_t>(_max_column)) + static_cast<size_t>(col);
      row_block.push_back(&(_map.at(index)));
    }

    _row_block.at(static_cast<size_t>(row)) = row_block;
  }

  for (int row = 0; row < _max_column; ++row) {
    for (int col = 0; col < _max_column; ++col) {
      const auto index =
          (static_cast<size_t>(row) * static_cast<size_t>(_max_column)) + static_cast<size_t>(col);
      _xy_block.at(static_cast<size_t>(row / BOX_SIZE))
          .at(static_cast<size_t>(col / BOX_SIZE))
          .push_back(&(_map.at(index)));
    }
  }
}

bool CScene::setCurValue(int nCurValue, int& nLastValue) {
  const auto index =
      static_cast<size_t>(_cur_point.x) + (static_cast<size_t>(_cur_point.y) * GRID_SIZE);
  const auto point = _map.at(index);
  if (point.state == State::ERASED) {
    nLastValue = point.value;
    setValue(nCurValue);
    return true;
  }

  return false;
}

void CScene::setValue(const point_t& point, int value) {
  const auto index = static_cast<size_t>(point.x) + (static_cast<size_t>(point.y) * GRID_SIZE);
  _map.at(index).value = value;
}

void CScene::setValue(int value) {
  auto point = _cur_point;
  this->setValue(point, value);
}

// Clear `count` random cells
void CScene::eraseRandomGrids(int count) {
  point_value_t erased_value = {UNSELECTED, State::ERASED};

  std::vector<int> cell_indexes(CELL_COUNT);
  for (int i = 0; i < CELL_COUNT; ++i) {
    cell_indexes[i] = i;
  }

  for (int i = 0; i < count; ++i) {
    const auto random_index =
        static_cast<size_t>(RandomInt(0, static_cast<int>(cell_indexes.size() - 1)));
    _map.at(static_cast<size_t>(cell_indexes.at(random_index))) = erased_value;
    cell_indexes.erase(cell_indexes.begin() +
                       static_cast<std::vector<int>::difference_type>(random_index));
  }
}

bool CScene::isComplete() {
  // If any block is not fully filled, the puzzle is not complete
  for (size_t i = 0; i < _map.size(); ++i) {
    if (_map.at(i).value == UNSELECTED) {
      return false;
    }
  }

  // Numbers in each block must also satisfy Sudoku rules
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int col = 0; col < GRID_SIZE; ++col) {
      if (!_row_block.at(static_cast<size_t>(row)).isValid() ||
          !_column_block.at(static_cast<size_t>(col)).isValid() ||
          !_xy_block.at(static_cast<size_t>(row / BOX_SIZE))
               .at(static_cast<size_t>(col / BOX_SIZE))
               .isValid()) {
        return false;
      }
    }
  }

  return true;
}

bool CScene::save(const char* filename) {
  {
    std::fstream file_stream;
    file_stream.open(filename, std::fstream::in);
    if (file_stream.is_open()) {
      file_stream.close();
      return false;
    }
  }

  std::fstream file_stream;
  file_stream.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

  // save _map
  for (size_t i = 0; i < _map.size(); ++i) {
    file_stream << _map.at(i).value << ' ' << static_cast<int>(_map.at(i).state) << '\n';
  }

  // save _cur_point
  file_stream << _cur_point.x << ' ' << _cur_point.y << '\n';

  // save _vCommand
  file_stream << _vCommand.size() << '\n';
  for (const CCommand& command : _vCommand) {
    point_t point = command.getPoint();
    file_stream << point.x << ' ' << point.y << ' ' << command.getPreValue() << ' '
                << command.getCurValue() << '\n';
  }

  file_stream.close();
  return true;
}

bool CScene::load(const char* filename) {
  auto filepath = std::filesystem::path(filename);
  if (!std::filesystem::exists(filepath)) {
    return false;
  }

  std::fstream file_stream;
  file_stream.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

  // load _map
  for (size_t i = 0; i < _map.size(); ++i) {
    int tmpState;
    file_stream >> _map.at(i).value >> tmpState;
    _map.at(i).state = static_cast<State>(tmpState);
  }

  // load _cur_point
  file_stream >> _cur_point.x >> _cur_point.y;

  // load _vCommand
  int commandSize;
  file_stream >> commandSize;
  for (int i = 0; i < commandSize; i++) {
    point_t point;
    int preValue;
    int curValue;
    file_stream >> point.x >> point.y >> preValue >> curValue;
    _vCommand.emplace_back(this, point, preValue, curValue);
  }
  return true;
}

void CScene::play() {
  show();

  char key = '\0';
  while (true) {
    key = static_cast<char>(_getch());
#ifdef _WIN32
    // Arrow keys emit an extended key prefix (0x00 or 0xE0), followed by the scan code.
    if (key == static_cast<char>(0x00) || key == static_cast<char>(0xE0)) {
      key = static_cast<char>(_getch());
    }
#endif
    if (key >= '0' && key <= '9') {
      CCommand oCommand(this);
      if (!oCommand.execute(key - '0')) {
        std::cout << "this number can't be modified." << '\n';
      } else {
        _vCommand.push_back(std::move(oCommand));  // XXX: move without move constructor
        show();
        continue;
      }
    }
    if (key == KEY_ESC) {
      Message(I18n::Instance().Get(I18n::Key::ASK_QUIT));
      std::string strInput;
      std::cin >> strInput;
      if (strInput[0] == 'y' || strInput[0] == 'Y') {
        Message(I18n::Instance().Get(I18n::Key::ASK_SAVE));
        std::cin >> strInput;
        if (strInput[0] == 'y' || strInput[0] == 'Y') {
          do {
            Message(I18n::Instance().Get(I18n::Key::ASK_SAVE_PATH));
            std::cin >> strInput;
            if (!save(strInput.c_str())) {
              Message(I18n::Instance().Get(I18n::Key::FILE_EXIST_ERROR));
            } else {
              break;
            }
          } while (true);
        }
        exit(0);
      } else {
        Message(I18n::Instance().Get(I18n::Key::CONTINUE));
      }
    } else if (key == KEY_UNDO) {
      if (_vCommand.empty()) {
        Message(I18n::Instance().Get(I18n::Key::UNDO_ERROR));
      } else {
        CCommand& oCommand = _vCommand.back();
        oCommand.undo();
        _vCommand.pop_back();
        show();
      }
    } else if (key == KEY_LEFT) {
      _cur_point.x = (_cur_point.x - 1) < 0 ? 0 : _cur_point.x - 1;
      show();
    } else if (key == KEY_RIGHT) {
      _cur_point.x = (_cur_point.x + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : _cur_point.x + 1;
      show();
    } else if (key == KEY_DOWN) {
      _cur_point.y = (_cur_point.y + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : _cur_point.y + 1;
      show();
    } else if (key == KEY_UP) {
      _cur_point.y = (_cur_point.y - 1) < 0 ? 0 : _cur_point.y - 1;
      show();
    } else if (key == KEY_ENTER) {
      if (isComplete()) {
        Message(I18n::Instance().Get(I18n::Key::CONGRATULATION));
        getchar();
        exit(0);
      } else {
        Message(I18n::Instance().Get(I18n::Key::NOT_COMPLETED));
      }
    }
  }
}

// A scene can be initialized multiple times
void CScene::generate() {
  std::vector<std::vector<int>> matrix(static_cast<size_t>(GRID_SIZE),
                                       std::vector<int>(static_cast<size_t>(GRID_SIZE), 0));

  // Initialize three 3x3 units
  // 2 6 5 | 0 0 0 | 0 0 0
  // 3 4 1 | 0 0 0 | 0 0 0
  // 8 9 7 | 0 0 0 | 0 0 0
  // ---------------------
  // 0 0 0 | 1 9 4 | 0 0 0
  // 0 0 0 | 8 3 6 | 0 0 0
  // 0 0 0 | 5 2 7 | 0 0 0
  // ---------------------
  // 0 0 0 | 0 0 0 | 3 4 5
  // 0 0 0 | 0 0 0 | 9 6 2
  // 0 0 0 | 0 0 0 | 7 8 1
  for (int num = 0; num < BOX_SIZE; ++num) {
    std::vector<int> unit = ShuffleUnit();
    const int start_index = num * BOX_SIZE;
    for (int i = start_index; i < start_index + BOX_SIZE; ++i) {
      for (int j = start_index; j < start_index + BOX_SIZE; ++j) {
        matrix[i][j] = unit.back();
        unit.pop_back();
      }
    }
  }

  // Count empty cells
  std::vector<std::tuple<int, int>> box_list;
  box_list.reserve(static_cast<size_t>(CELL_COUNT));
  for (int i = 0; i < GRID_SIZE; ++i) {
    for (int j = 0; j < GRID_SIZE; ++j) {
      if (matrix[i][j] == 0) {
        box_list.push_back(std::make_tuple(i, j));
      }
    }
  }

  // Fill empty cells one by one
  std::map<std::string, std::vector<int>> available_num{};
  int full_num = 0;
  int empty_num = static_cast<int>(box_list.size());
  while (full_num < empty_num) {
    std::tuple<int, int> position = box_list[full_num];
    int row = std::get<0>(position);
    int col = std::get<1>(position);
    std::vector<int> able_unit;
    std::string key = std::to_string(row) + "x" + std::to_string(col);
    if (available_num.find(key) == available_num.end()) {
      // 3x3 box
      able_unit = GetUnit();
      for (int i = row / BOX_SIZE * BOX_SIZE; i < row / BOX_SIZE * BOX_SIZE + BOX_SIZE; ++i) {
        for (int j = col / BOX_SIZE * BOX_SIZE; j < col / BOX_SIZE * BOX_SIZE + BOX_SIZE; ++j) {
          able_unit.erase(std::remove(able_unit.begin(), able_unit.end(), matrix[i][j]),
                          able_unit.end());
        }
      }
      // Row
      for (int i = 0; i < GRID_SIZE; ++i) {
        if (matrix[row][i] != 0) {
          able_unit.erase(std::remove(able_unit.begin(), able_unit.end(), matrix[row][i]),
                          able_unit.end());
        }
      }
      // Column
      for (int i = 0; i < GRID_SIZE; ++i) {
        if (matrix[i][col] != 0) {
          able_unit.erase(std::remove(able_unit.begin(), able_unit.end(), matrix[i][col]),
                          able_unit.end());
        }
      }
      available_num[key] = able_unit;
    } else {
      able_unit = available_num[key];
    }

    // Backtrack if there is no available number
    if (available_num[key].empty()) {
      full_num -= 1;
      if (available_num.find(key) != available_num.end()) {
        available_num.erase(key);
      }
      matrix[row][col] = 0;
      continue;
    }

    matrix[row][col] = available_num[key].back();
    available_num[key].pop_back();
    full_num += 1;
  }

  // Write generated values into the scene
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int col = 0; col < GRID_SIZE; ++col) {
      point_t point = {row, col};
      setValue(point, matrix[row][col]);
    }
  }

  assert(isComplete());
}

bool CScene::setPointValue(const point_t& stPoint, int nValue) {
  const auto index = static_cast<size_t>(stPoint.x) + (static_cast<size_t>(stPoint.y) * GRID_SIZE);
  const auto point = _map.at(index);
  if (State::ERASED == point.state) {
    _cur_point = stPoint;
    setValue(nValue);
    return true;
  }

  return false;
}

point_t CScene::getCurPoint() { return _cur_point; }
