#include <cstring>
#include <iostream>

#include "i18n.h"
#include "input.h"
#include "scene.h"
#include "system_env.hpp"
#include "utility.inl"

static void printHelp() {
  std::cout << '\n';
  std::cout << "sudoku - a little game in command line\n\n";
  std::cout << "Usage:\n";
  std::cout << "\t sudoku [-l <progressFile>]\n\n";
  std::cout << "Options:\n";
  std::cout << "\t -l <path> \t specify path of progress file to load, optional.\n\n";
}

int main(int argc, char** argv) {
  SetSystemEnv();

  CScene scene;

  if (argc == 1) {
    InputLanguage();
    int eraseGridNumber = inputDifficulty();
    scene.generate();
    scene.eraseRandomGrids(eraseGridNumber);
  } else if (argc == 3 && (std::strcmp(argv[1], "-l") == 0)) {
    // load saved game progress
    if (!scene.load(argv[2])) {
      Message(I18n::Instance().Get(I18n::Key::LOAD_PROGRESS_FAIL));
      return 0;
    }
    InputLanguage();
  } else {
    printHelp();
    return 0;
  }

  scene.play();

  return 0;
}