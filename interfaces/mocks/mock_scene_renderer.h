#pragma once

#include <gmock/gmock.h>

#include "board.h"
#include "i_scene_renderer.h"

class MockSceneRenderer : public ISceneRenderer {
 public:
  MOCK_METHOD(void, Render,
              (const Board& board, const point_t& cursor_point, int max_column),
              (const, override));
};
