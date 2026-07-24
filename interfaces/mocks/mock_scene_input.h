#pragma once

#include <gmock/gmock.h>

#include <string>

#include "i_scene_input.h"

class MockSceneInput : public ISceneInput {
 public:
  MOCK_METHOD(char, ReadKey, (), (override));
  MOCK_METHOD(std::string, ReadToken, (), (override));
  MOCK_METHOD(void, WaitForKey, (), (override));
};
