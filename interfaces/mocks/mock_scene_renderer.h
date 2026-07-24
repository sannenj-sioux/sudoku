#pragma once

#include <gmock/gmock.h>

#include <string>

#include "board.h"
#include "i_scene_renderer.h"

class MockSceneRendererBase : public ISceneRenderer {
 public:
  MOCK_METHOD(void, Render,
              (const Board& board, const point_t& cursor_point, int max_column),
              (const, override));
    MOCK_METHOD(void, RenderMessage, (const std::string& message, bool add_line_feed), (const, override));
};

using MockSceneRenderer = ::testing::NiceMock<MockSceneRendererBase>;
