#pragma once

#include <iostream>

#if _WIN32
#include <Windows.h>
#endif

inline void SetWindowsEnv() {
#if _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif
}

inline void SetSystemEnv() {
  SetWindowsEnv();
  // Disable output buffering to improve output speed and reduce stutter
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}
