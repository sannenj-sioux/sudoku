#pragma once

#include "common.h"

/**
 * @brief Interface used by command objects to manipulate scene state.
 */
class ISceneCommand {
 public:
  virtual ~ISceneCommand() = default;

  /**
   * @brief Set value at the current cursor point if editable.
   * @param nCurValue Value to write.
   * @param nLastValue Previous value at the cursor point (output).
   * @return True if value was set; otherwise false.
   */
  virtual bool setCurValue(int nCurValue, int& nLastValue) = 0;

  /**
   * @brief Set value at a specific point if editable.
   * @param point Target board coordinate.
   * @param value Value to write.
   * @return True if value was set; otherwise false.
   */
  virtual bool setPointValue(const point_t& point, int value) = 0;

  /**
   * @brief Get current cursor point.
   * @return Current cursor coordinate.
   */
  virtual point_t getCurPoint() = 0;
};
