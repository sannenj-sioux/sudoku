#include "console_scene_input.h"

#include <iostream>

#include "utility.inl"

char ConsoleSceneInput::ReadKey() { return static_cast<char>(_getch()); }

std::string ConsoleSceneInput::ReadToken() {
  std::string token;
  std::cin >> token;
  return token;
}

void ConsoleSceneInput::WaitForKey() { static_cast<void>(getchar()); }
