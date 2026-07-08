#pragma once

/**
 * @brief Interface for Sudoku scene/gameplay operations.
 */
class IScene {
 public:
  virtual ~IScene() = default;

  /**
   * @brief Generate a solved Sudoku board.
   */
  virtual void generate() = 0;

  /**
   * @brief Erase a number of cells to create a puzzle.
   * @param count Number of cells to erase.
   */
  virtual void eraseRandomGrids(int count) = 0;

  /**
   * @brief Check whether the current board is fully and validly solved.
   * @return True if the puzzle is complete and valid; otherwise false.
   */
  virtual bool isComplete() = 0;

  /**
   * @brief Run the interactive gameplay loop.
   */
  virtual void play() = 0;

  /**
   * @brief Save current game state to a file.
   * @param filename Destination file path.
   * @return True if save succeeds; otherwise false.
   */
  virtual bool save(const char* filename) = 0;

  /**
   * @brief Load game state from a file.
   * @param filename Source file path.
   * @return True if load succeeds; otherwise false.
   */
  virtual bool load(const char* filename) = 0;
};
