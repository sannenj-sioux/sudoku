#include "game.h"

#include <memory>

#include "i18n.h"
#include "input.h"
#include "scene.h"
#include "system_env.hpp"
#include "utility.inl"

Game::Game() : scene_(std::make_unique<CScene>()) {
  SetSystemEnv();
}

int Game::StartNewGame() {
  InputLanguage();
  const int erase_grid_number = inputDifficulty();
  scene_->generate();
  scene_->eraseRandomGrids(erase_grid_number);
  scene_->play();
  return 0;
}

int Game::LoadGame(const char* progress_file_path) {
  if (!scene_->load(progress_file_path)) {
    Message(I18n::Instance().Get(I18n::Key::LOAD_PROGRESS_FAIL));
    return 0;
  }

  InputLanguage();
  scene_->play();
  return 0;
}