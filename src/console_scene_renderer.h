#pragma once

#include <string>

#include "i_scene_renderer.h"

class ConsoleSceneRenderer : public ISceneRenderer {
 public:
  void Render(const Board& board, const point_t& cursor_point, int max_column) const override;
  void RenderMessage(const std::string& message, bool add_line_feed = true) const override;
};
