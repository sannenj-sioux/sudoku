#pragma once

#include <array>
#include <vector>

#include "board.h"
#include "block.h"
#include "command.h"
#include "common.h"
#include "i_puzzle_generator.h"

// Sudoku scene class
class CScene {
 public:
  CScene(int index = 3, IPuzzleGenerator* puzzle_generator = nullptr);
  virtual ~CScene();

  void generate();
  void show() const;

  point_value_t getPointValue(const point_t&) const;
  bool setCurValue(int nCurValue, int& nLastValue);
  bool setPointValue(const point_t&, int);
  point_t getCurPoint();

  void eraseRandomGrids(int count);
  bool isComplete();

  void play();
  bool save(const char* filename);
  bool load(const char* filename);

 private:
  void init();  // Put each cell pointer into its corresponding block
  void setValue(int);
  void setValue(const point_t&, int);
  void printUnderline(int line_no = -1) const;

  int _max_column;
  point_t _cur_point;
  std::array<CBlock, GRID_SIZE> _column_block;
  std::array<CBlock, GRID_SIZE> _row_block;
  std::array<std::array<CBlock, BOX_SIZE>, BOX_SIZE> _xy_block;
  Board _board;
  IPuzzleGenerator* _puzzle_generator;

  std::vector<CCommand> _vCommand;
};
