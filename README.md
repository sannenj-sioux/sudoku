# sudoku
A cross-platform Sudoku game implemented in C++, with beginner-friendly command-line controls. It is great for relaxing during short development breaks. The codebase is only a few hundred lines, so beginners can understand it easily.
Contributions are welcome through pull requests for new features or bug fixes.

## Thanks To Contributors
<a href="https://github.com/mayerui/sudoku/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=mayerui/sudoku" />
</a>

## Features
1. Cross-platform / compiler support: Linux/Windows/macOS [![Linux](https://github.com/mayerui/sudoku/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/mayerui/sudoku/actions/workflows/ci-linux.yml) [![Windows](https://github.com/mayerui/sudoku/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/mayerui/sudoku/actions/workflows/ci-windows.yml) [![macOS](https://github.com/mayerui/sudoku/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/mayerui/sudoku/actions/workflows/ci-macos.yml)
2. Multilingual support: English/Chinese
3. No third-party library dependencies
4. Runs in the console

## Requirements
1. cmake 3.12+
2. C++17

## Build
``` shell
cmake -B build -S .
cmake --build build
```

## Run
The build step generates the `sudoku` executable in the `bin` directory.
``` shell
./sudoku  # Start directly
./sudoku -l filename  # Load a saved game progress file
./sudoku -h  # Show help information
```

## Controls
- 0 Delete an entered number
- u Undo the previous action
- enter Try to complete the puzzle
- esc Exit the game

### Normal Mode
- w Move cursor up ↑
- a Move cursor left ←
- s Move cursor down ↓
- d Move cursor right →

### Vim Mode
- k Move cursor up ↑
- h Move cursor left ←
- j Move cursor down ↓
- l Move cursor right →

## Project Structure
```bash
│--.gitignore  
│--build.bat        // One-click build script for Windows  
│--build.sh         // One-click build script for Linux/macOS  
│--CMakeLists.txt   // CMake project file  
│--README.md     
└--src              // Source code directory  
  │--block.cpp     // Sudoku block combination class; can represent rows, columns, and 3x3 boxes  
   │--block.h  
  │--color.h       // Color class  
  │--command.cpp   // Command class with undo support  
   │--command.h     
  │--common.h      // Shared header file  
  │--input.cpp     // Input class  
   │--input.h   
  │--main.cpp      // Entry point  
  │--scene.cpp     // Game scene class  
   │--scene.h   
  │--test.cpp      // Test file  
   │--test.h  
  └--utility.inl   // Some useful global utility functions  
```
