#pragma once

#include "common.h"
#include "i_scene_command.h"

class CCommand {
 public:
  CCommand(ISceneCommand* pOwner);
  CCommand(ISceneCommand* pOwner, const point_t& point, int preValue, int curValue);
  CCommand(const CCommand&);
  ~CCommand();

  bool execute(int nInputValue);
  void undo();
  point_t getPoint() const { return _stPoint; }
  int getPreValue() const { return _nPreValue; }
  int getCurValue() const { return _nCurValue; }
  void setPoint(const point_t& point) { _stPoint = point; }
  void setPreValue(int preValue) { _nPreValue = preValue; }
  void setCurValue(int curValue) { _nCurValue = curValue; }

 private:
  ISceneCommand* _pOwner;
  point_t _stPoint;
  int _nPreValue;
  int _nCurValue;  // actually the member is never used
};
