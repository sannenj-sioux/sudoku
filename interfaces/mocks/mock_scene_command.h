#pragma once

#include <gmock/gmock.h>

#include "i_scene_command.h"

class MockSceneCommand : public ISceneCommand {
 public:
  MOCK_METHOD(bool, setCurValue, (int nCurValue, int& nLastValue), (override));
  MOCK_METHOD(bool, setPointValue, (const point_t& point, int value), (override));
  MOCK_METHOD(point_t, getCurPoint, (), (override));
};
