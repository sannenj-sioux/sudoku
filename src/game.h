#pragma once

#include "scene.h"

class Game {
 public:
  Game();

  int StartNewGame();
  int LoadGame(const char* progress_file_path);

 private:
  CScene scene_;
};