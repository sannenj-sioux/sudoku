#pragma once

class Board;

class IPuzzleGenerator {
 public:
  virtual ~IPuzzleGenerator() = default;
  virtual void GenerateSolvedBoard(Board& board) = 0;
  virtual void EraseCells(Board& board, int count) = 0;
};