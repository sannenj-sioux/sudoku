#pragma once

#include "board.h"

class PuzzleGenerator {
 public:
  static Board GenerateSolvedBoard();
  static void EraseCells(Board& board, int count);
};