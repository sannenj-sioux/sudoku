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
generate_uml.bat
```

This will:

1. Locate Visual Studio C++ build tools
2. Configure `build-uml` with Ninja and export `compile_commands.json`
3. Run `clang-uml` with `.clang-uml`
4. Write output files to the `uml` folder

## Configuration

- Config file: `.clang-uml`
- Diagram name: `sudoku_class`
- Input scope: `src/*.h`, `src/*.hpp`, `src/*.cpp`

Adjust `.clang-uml` if you want to limit or expand coverage.
