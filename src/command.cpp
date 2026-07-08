#include "command.h"

CCommand::CCommand(ISceneCommand* pOwner) : _pOwner(pOwner) {}

CCommand::CCommand(ISceneCommand* pOwner, const point_t& point, int preValue, int curValue)
    : _pOwner(pOwner), _stPoint(point), _nPreValue(preValue), _nCurValue(curValue) {}

CCommand::CCommand(const CCommand& rhs)
    : _pOwner(rhs._pOwner),
      _stPoint(rhs._stPoint),
      _nPreValue(rhs._nPreValue),
      _nCurValue(rhs._nCurValue) {}

CCommand::~CCommand() {}

bool CCommand::execute(int nInputValue) {
  if (_pOwner == nullptr) {
    return false;
  }

  _stPoint = _pOwner->getCurPoint();
  return _pOwner->setCurValue(nInputValue, _nPreValue);
}

void CCommand::undo() {
  if (_pOwner != nullptr) {
    _pOwner->setPointValue(_stPoint, _nPreValue);
  }
}