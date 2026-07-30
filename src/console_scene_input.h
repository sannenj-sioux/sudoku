#pragma once

#include "i_scene_input.h"

class ConsoleSceneInput : public ISceneInput {
 public:
  char ReadKey() override;
  std::string ReadToken() override;
  void WaitForKey() override;
};
