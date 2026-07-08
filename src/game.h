#pragma once

#include <memory>

#include "i_scene.h"

class Game {
 public:
  Game();

  int StartNewGame();
  int LoadGame(const char* progress_file_path);

 private:
    std::unique_ptr<IScene> scene_;
};