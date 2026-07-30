#pragma once

#include <gmock/gmock.h>

#include "i_scene.h"

class MockScene : public IScene {
 public:
  MOCK_METHOD(void, generate, (), (override));
  MOCK_METHOD(void, eraseRandomGrids, (int count), (override));
  MOCK_METHOD(bool, isComplete, (), (override));
  MOCK_METHOD(void, play, (), (override));
  MOCK_METHOD(bool, save, (const char* filename), (override));
  MOCK_METHOD(bool, load, (const char* filename), (override));
};
