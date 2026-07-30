#pragma once

#include "i_puzzle_generator.h"

class PuzzleGenerator : public IPuzzleGenerator {
 public:
#pragma region IPuzzleGenerator
  void GenerateSolvedBoard(Board& board) override;
  void EraseCells(Board& board, int count) override;
#pragma endregion
};