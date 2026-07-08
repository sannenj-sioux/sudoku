#pragma once

#include "i_puzzle_generator.h"

class PuzzleGenerator : public IPuzzleGenerator {
 public:
  void GenerateSolvedBoard(Board& board) override;
  void EraseCells(Board& board, int count) override;
};