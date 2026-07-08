#include <cstring>
#include <iostream>

#include "game.h"

static void printHelp() {
  std::cout << '\n';
  std::cout << "sudoku - a little game in command line\n\n";
  std::cout << "Usage:\n";
  std::cout << "\t sudoku [-l <progressFile>]\n\n";
  std::cout << "Options:\n";
  std::cout << "\t -l <path> \t specify path of progress file to load, optional.\n\n";
}

int main(int argc, char** argv) {
  Game game;

  if (argc == 1) {
    return game.StartNewGame();
  }

  if (argc == 3 && (std::strcmp(argv[1], "-l") == 0)) {
    return game.LoadGame(argv[2]);
  } else {
    printHelp();
    return 0;
  }
}