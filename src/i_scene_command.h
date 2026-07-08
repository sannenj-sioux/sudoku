#pragma once

#include "common.h"

class ISceneCommand {
 public:
  virtual ~ISceneCommand() = default;

  virtual bool setCurValue(int nCurValue, int& nLastValue) = 0;
  virtual bool setPointValue(const point_t& point, int value) = 0;
  virtual point_t getCurPoint() = 0;
};
