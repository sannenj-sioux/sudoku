#pragma once

class Board;

/**
 * @brief Interface for Sudoku board generation and puzzle carving.
 */
class IPuzzleGenerator {
 public:
  virtual ~IPuzzleGenerator() = default;

  /**
   * @brief Fill the board with a valid solved Sudoku configuration.
   * @param board Board instance to populate.
   */
  virtual void GenerateSolvedBoard(Board& board) = 0;

  /**
   * @brief Erase cells on a solved board to create a playable puzzle.
   * @param board Board instance to modify.
   * @param count Number of cells to erase.
   */
  virtual void EraseCells(Board& board, int count) = 0;
};
