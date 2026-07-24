#pragma once

#include <string>

#include "common.h"

class Board;

/**
 * @brief Interface for rendering a scene to the UI.
 */
class ISceneRenderer {
 public:
  virtual ~ISceneRenderer() = default;

  /**
   * @brief Render the current board state and cursor location.
   */
  virtual void Render(const Board& board, const point_t& cursor_point, int max_column) const = 0;

  /**
   * @brief Render a scene message (prompts, errors, and status text).
   */
  virtual void RenderMessage(const std::string& message, bool add_line_feed = true) const = 0;
};
