# UML generation workflow (clang-uml)

This repository includes a minimal setup to reverse engineer C++ classes into UML diagrams.

## Prerequisites

1. CMake (already handled by `install_cmake.bat`)
2. clang-uml + Ninja

Install tools:

```bat
install_clang_uml.bat
```

## Generate UML

Run from repository root:

```bat
cmake -S . -B build
cmake --build build --target generate_uml
```

This will:

1. Refresh `compile_commands.json` (for Visual Studio generators)
2. Run `clang-uml` with `.clang-uml`
3. Write output files to the `uml` folder

## Configuration

- Config file: `.clang-uml`
- Diagram name: `sudoku_class`
- Input scope: `src/*.h`, `src/*.hpp`, `src/*.cpp`
- Build target: `generate_uml`

Adjust `.clang-uml` if you want to limit or expand coverage.
