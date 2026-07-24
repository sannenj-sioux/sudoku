#pragma once

#include <vector>

#include "board.h"
#include "command.h"
#include "common.h"
#include "i_scene_input.h"
#include "i_puzzle_generator.h"
#include "i_scene_renderer.h"
#include "i_scene.h"
#include "i_scene_command.h"

// Sudoku scene class
class CScene : public IScene, public ISceneCommand {
 public:
  CScene(int index = 3, IPuzzleGenerator* puzzle_generator = nullptr,
    const ISceneRenderer* scene_renderer = nullptr, ISceneInput* scene_input = nullptr);
  virtual ~CScene();

#pragma region IScene
  void generate() override;
  void eraseRandomGrids(int count) override;
  bool isComplete() override;
  void play() override;
  bool save(const char* filename) override;
  bool load(const char* filename) override;
#pragma endregion

#pragma region ISceneCommand
  bool setCurValue(int nCurValue, int& nLastValue) override;
  bool setPointValue(const point_t& point, int value) override;
  point_t getCurPoint() override;
#pragma endregion

 protected:
  void show() const;

 private:

  void init();  // Put each cell pointer into its corresponding block
  void setValue(int);
  void setValue(const point_t&, int);

  int _max_column;
  point_t _cur_point;
  Board _board;
  IPuzzleGenerator* _puzzle_generator;
  const ISceneRenderer* _scene_renderer;
  ISceneInput* _scene_input;

  std::vector<CCommand> _vCommand;
};
