#pragma once

#include <string>

/**
 * @brief Interface for scene user input and process control.
 */
class ISceneInput {
 public:
  virtual ~ISceneInput() = default;

  /**
   * @brief Read one input key (supports special key prefixes where applicable).
   */
  virtual char ReadKey() = 0;

  /**
   * @brief Read one token from standard input.
   */
  virtual std::string ReadToken() = 0;

  /**
   * @brief Wait for a user keystroke.
   */
  virtual void WaitForKey() = 0;
};
