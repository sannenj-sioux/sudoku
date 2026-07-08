#include <iostream>
#include <string>

#include "common.h"
#include "i18n.h"
#include "utility.inl"

// return number of grids to be erased
int inputDifficulty() {
  ClearScreen();

  std::string cmd;
  int need_erase_grids = 0;
  while (true) {
    Message(I18n::Instance().Get(I18n::Key::ASK_DIFFICULTY));

    std::cin >> cmd;

    try {
      Difficulty difficulty = static_cast<Difficulty>(std::stoi(cmd));
      switch (difficulty) {
        case Difficulty::EASY:
          need_erase_grids = 20;
          break;
        case Difficulty::NORMAL:
          need_erase_grids = 35;
          break;
        case Difficulty::HARD:
          need_erase_grids = 50;
          break;
      }
    } catch (const std::exception&) {
      need_erase_grids = 0;
    }

    if (need_erase_grids > 0) {
      break;
    }

    Message(I18n::Instance().Get(I18n::Key::INPUT_ERROR));
  }

  return need_erase_grids;
}

void InputLanguage() {
  std::string language;
  do {
    Message("Language: 1 English, 2 中文");
    std::cin >> language;

    try {
      auto selected_language = static_cast<Language>(std::stoul(language) - 1);
      if (selected_language < Language::MAX) {
        I18n::Instance().SetLanguage(selected_language);
        return;
      }
    } catch (const std::exception& exception) {
      static_cast<void>(exception);
      // Fall through to show input error.
    }

    Message(I18n::Instance().Get(I18n::Key::INPUT_ERROR));
  } while (true);
}
