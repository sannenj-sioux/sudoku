#pragma once

#include <cstdint>
#include <map>
#include <string>

enum class Language : std::uint8_t {
  ENGLISH,
  CHINESE,
  MAX,
};

class I18n {
 public:
  enum class Key : std::uint8_t {
    INPUT_ERROR,
    LOAD_PROGRESS_FAIL,
    ASK_QUIT,
    ASK_SAVE,
    ASK_SAVE_PATH,
    FILE_EXIST_ERROR,
    CONTINUE,
    UNDO_ERROR,
    CONGRATULATION,
    NOT_COMPLETED,
    ASK_DIFFICULTY,
  };
  using Dict = std::map<Key, std::string>;

  static I18n& Instance();
  void SetLanguage(Language);
  std::string Get(Key) const;

 private:
  I18n();

  const Dict* dict_;
};
