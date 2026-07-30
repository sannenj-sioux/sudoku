#include "scene.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>

#include "console_scene_input.h"
#include "console_scene_renderer.h"
#include "i18n.h"
#include "key_definitions.h"
#include "puzzle_generator.h"

namespace {
PuzzleGenerator kDefaultPuzzleGenerator;
ConsoleSceneRenderer kDefaultSceneRenderer;
ConsoleSceneInput kDefaultSceneInput;
}  // namespace

CScene::CScene(int index, IPuzzleGenerator* puzzle_generator, const ISceneRenderer* scene_renderer,
               ISceneInput* scene_input)
    : _max_column(static_cast<int>(pow(index, 2))),
      _cur_point({0, 0}),
      _puzzle_generator(puzzle_generator != nullptr ? puzzle_generator : &kDefaultPuzzleGenerator),
      _scene_renderer(scene_renderer != nullptr ? scene_renderer : &kDefaultSceneRenderer),
      _scene_input(scene_input != nullptr ? scene_input : &kDefaultSceneInput) {
  init();
}

CScene::~CScene() = default;

void CScene::show() const {
  _scene_renderer->Render(_board, _cur_point, _max_column);
}

void CScene::init() {
  _board.reset();
}

bool CScene::setCurValue(int nCurValue, int& nLastValue) {
  const auto index =
      static_cast<size_t>(_cur_point.x) + (static_cast<size_t>(_cur_point.y) * GRID_SIZE);
  const auto point = _board.at(index);
  if (point.state == State::ERASED) {
    nLastValue = point.value;
    setValue(nCurValue);
    return true;
  }

  return false;
}

void CScene::setValue(const point_t& point, int value) {
  const auto index = static_cast<size_t>(point.x) + (static_cast<size_t>(point.y) * GRID_SIZE);
  _board.at(index).value = value;
  _board.refreshValidationState();
}

void CScene::setValue(int value) {
  auto point = _cur_point;
  this->setValue(point, value);
}

void CScene::eraseRandomGrids(int count) {
  _puzzle_generator->EraseCells(_board, count);
  _board.refreshValidationState();
}

bool CScene::isComplete() {
  _board.refreshValidationState();

  if (!_board.isValidState()) {
    return false;
  }

  const auto& row_blocks = _board.rowBlocks();
  const auto& column_blocks = _board.columnBlocks();
  const auto& box_blocks = _board.boxBlocks();

  // If any block is not fully filled, the puzzle is not complete
  for (size_t i = 0; i < _board.size(); ++i) {
    if (_board.at(i).value == UNSELECTED) {
      return false;
    }
  }

  // Numbers in each block must also satisfy Sudoku rules
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int col = 0; col < GRID_SIZE; ++col) {
      if (!row_blocks.at(static_cast<size_t>(row)).isValid() ||
          !column_blocks.at(static_cast<size_t>(col)).isValid() ||
          !box_blocks.at(static_cast<size_t>(row / BOX_SIZE))
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

  // save board cells
  for (size_t i = 0; i < _board.size(); ++i) {
    file_stream << _board.at(i).value << ' ' << static_cast<int>(_board.at(i).state) << '\n';
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

  // load board cells
  for (size_t i = 0; i < _board.size(); ++i) {
    int tmpState;
    file_stream >> _board.at(i).value >> tmpState;
    _board.at(i).state = static_cast<State>(tmpState);
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
  _board.refreshValidationState();
  return true;
}

void CScene::play() {
  show();

  char key = '\0';
  while (true) {
    key = _scene_input->ReadKey();
#ifdef _WIN32
    // Arrow keys emit an extended key prefix (0x00 or 0xE0), followed by the scan code.
    if (key == SceneKeys::kExtendedPrefix0 || key == SceneKeys::kExtendedPrefixE0) {
      key = _scene_input->ReadKey();
    }
#endif
    if (key >= '0' && key <= '9') {
      CCommand oCommand(this);
      if (!oCommand.execute(key - '0')) {
        _scene_renderer->RenderMessage("this number can't be modified.");
      } else {
        _vCommand.push_back(std::move(oCommand));  // XXX: move without move constructor
        show();
        continue;
      }
    }
    if (key == SceneKeys::kEsc) {
      _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::ASK_QUIT));
      std::string strInput = _scene_input->ReadToken();
      if (strInput[0] == 'y' || strInput[0] == 'Y') {
        _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::ASK_SAVE));
        strInput = _scene_input->ReadToken();
        if (strInput[0] == 'y' || strInput[0] == 'Y') {
          do {
            _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::ASK_SAVE_PATH));
            strInput = _scene_input->ReadToken();
            if (!save(strInput.c_str())) {
              _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::FILE_EXIST_ERROR));
            } else {
              break;
            }
          } while (true);
        }
        return;
      } else {
        _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::CONTINUE));
      }
    } else if (key == SceneKeys::kUndo) {
      if (_vCommand.empty()) {
        _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::UNDO_ERROR));
      } else {
        CCommand& oCommand = _vCommand.back();
        oCommand.undo();
        _vCommand.pop_back();
        show();
      }
    } else if (key == SceneKeys::kLeft) {
      _cur_point.x = (_cur_point.x - 1) < 0 ? 0 : _cur_point.x - 1;
      show();
    } else if (key == SceneKeys::kRight) {
      _cur_point.x = (_cur_point.x + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : _cur_point.x + 1;
      show();
    } else if (key == SceneKeys::kDown) {
      _cur_point.y = (_cur_point.y + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : _cur_point.y + 1;
      show();
    } else if (key == SceneKeys::kUp) {
      _cur_point.y = (_cur_point.y - 1) < 0 ? 0 : _cur_point.y - 1;
      show();
    } else if (key == SceneKeys::kEnter) {
      if (isComplete()) {
        _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::CONGRATULATION));
        _scene_input->WaitForKey();
        return;
      } else {
        _scene_renderer->RenderMessage(I18n::Instance().Get(I18n::Key::NOT_COMPLETED));
      }
    }
  }
}

void CScene::generate() {
  _puzzle_generator->GenerateSolvedBoard(_board);
  _board.refreshValidationState();

  assert(isComplete());
}

bool CScene::setPointValue(const point_t& stPoint, int nValue) {
  const auto index = static_cast<size_t>(stPoint.x) + (static_cast<size_t>(stPoint.y) * GRID_SIZE);
  const auto point = _board.at(index);
  if (State::ERASED == point.state) {
    _cur_point = stPoint;
    setValue(nValue);
    return true;
  }

  return false;
}

point_t CScene::getCurPoint() { return _cur_point; }
