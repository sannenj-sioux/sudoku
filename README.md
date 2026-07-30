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

## VS Code Tasks
- `CMake: configure`: Configure the CMake project into the `build` directory.
- `CMake: build`: Build the project using the configured generator.
- `CMake: clean`: Clean build outputs.
- `CMake: configure & build`: Run configure and build in sequence.
- `CTest: unit tests (Debug)`: Run all discovered unit tests in Debug config.
- `CTest: unit tests (Release)`: Run all discovered unit tests in Release config.
- `GTest: unit tests (Debug, colored)`: Run `sudoku_unit_tests` directly with colored output.
- `GTest: unit tests (Release, colored)`: Run `sudoku_unit_tests` in Release mode with colored output.
- `CMake: generate UML`: Generate UML diagrams via the `generate_uml` CMake target.
- `CMake: generate coverage (Debug)`: Generate HTML code coverage report via the `generate_coverage` CMake target.

## Requirements
1. cmake 3.12+
2. C++17

## Build
``` shell
cmake -S . -B build
cmake --build build --config Release
```

## Unit Tests (GoogleTest/GoogleMock)
The test target uses GoogleTest/GoogleMock via CMake `FetchContent`.
The dependency is downloaded automatically when CMake configures the project.
All unit test cases are linked into a single executable: `sudoku_unit_tests`.

``` shell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Run the test executable directly with colored output:

``` shell
# Windows (Visual Studio generator)
.\bin\Debug\sudoku_unit_tests.exe --gtest_color=yes
```

## Code Coverage (Windows)
Coverage is generated with OpenCppCoverage.

Install the tool:

```bat
install_opencppcoverage.bat
```

Generate HTML coverage from Debug tests via CMake:

```bat
cmake -S . -B build
cmake --build build --config Debug --target generate_coverage
```

Open the report in:

```text
coverage/html/index.html
```

In VS Code you can run the task `CMake: generate coverage (Debug)`.

## Run
The build step generates the `sudoku` executable in the `bin` directory.
``` shell
# Linux/macOS
./bin/sudoku
./bin/sudoku -l filename

# Windows (Visual Studio generator)
.\bin\Release\sudoku.exe
.\bin\Release\sudoku.exe -l filename
```

## Controls
- Arrow keys: move the cursor
- `1`-`9`: fill a number into the current cell (editable cells only)
- `0`: clear the current editable cell
- `u`: undo the previous action
- `Enter`: validate whether the puzzle is complete
- `Esc`: quit (with save prompt)

## UML Generation
UML generation is integrated into CMake through the `generate_uml` target.

``` shell
cmake --build build --config Release --target generate_uml
```

Generated diagrams are written to the `uml` directory.
For detailed setup notes, see `UML_WORKFLOW.md`.

## Project Structure
```bash
│--CMakeLists.txt
│--README.md
│--install_cmake.bat
│--install_clang_uml.bat
│--install_opencppcoverage.bat
│--interfaces/
│  │--i_puzzle_generator.h
│  │--i_scene.h
│  │--i_scene_command.h
│  └--mocks/
│     │--mock_puzzle_generator.h
│     │--mock_scene.h
│     └--mock_scene_command.h
│--src/
│  │--main.cpp        // Program entry point
│  │--scene.cpp/.h    // Game scene and interaction logic
│  │--input.cpp/.h    // Startup input flow (language/difficulty)
│  │--command.cpp/.h  // Undo command handling
│  │--block.cpp/.h    // Sudoku row/column/box abstraction
│  │--i18n.cpp/.h     // Localization support
│  └--...
│--tests/
│  │--puzzle_generation_test.cpp
│  │--scene_test.cpp
│  └--performance_test.cpp
└--uml/
   └--sudoku_class.puml
```
