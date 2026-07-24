#pragma once

#include "i_scene_renderer.h"

class ConsoleSceneRenderer : public ISceneRenderer {
 public:
  void Render(const Board& board, const point_t& cursor_point, int max_column) const override;
};
