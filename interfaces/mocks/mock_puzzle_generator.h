#pragma once

#include <gmock/gmock.h>

#include "i_puzzle_generator.h"

class MockPuzzleGenerator : public IPuzzleGenerator {
 public:
  MOCK_METHOD(void, GenerateSolvedBoard, (Board& board), (override));
  MOCK_METHOD(void, EraseCells, (Board& board, int count), (override));
};
