#pragma once

#include <vector>

#include "board.h"
#include "command.h"
#include "common.h"
#include "i_puzzle_generator.h"
#include "i_scene.h"
#include "i_scene_command.h"

// Sudoku scene class
class CScene : public IScene, public ISceneCommand {
 public:
  CScene(int index = 3, IPuzzleGenerator* puzzle_generator = nullptr);
  virtual ~CScene();

  void generate() override;

  void eraseRandomGrids(int count) override;
  bool isComplete() override;

  void play() override;
  bool save(const char* filename) override;
  bool load(const char* filename) override;

#pragma region ISceneCommand
  bool setCurValue(int nCurValue, int& nLastValue) override;
  bool setPointValue(const point_t&, int) override;
  point_t getCurPoint() override;
#pragma endregion

 private:
  void show() const;

  void init();  // Put each cell pointer into its corresponding block
  void setValue(int);
  void setValue(const point_t&, int);
  void printUnderline(int line_no = -1) const;

  int _max_column;
  point_t _cur_point;
  Board _board;
  IPuzzleGenerator* _puzzle_generator;

  std::vector<CCommand> _vCommand;
};
