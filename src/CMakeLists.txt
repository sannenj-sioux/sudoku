# CMakeLists.txt for the src directory

# Manually specify source files
set(SOURCE_FILES
    board.cpp
    block.cpp
    console_scene_input.cpp
    console_scene_renderer.cpp
    command.cpp
    game.cpp
    i18n.cpp
    input.cpp
    puzzle_generator.cpp
    scene.cpp
    main.cpp
)

add_executable(sudoku  # Output executable named sudoku
   ${SOURCE_FILES}
)

target_compile_features(sudoku PRIVATE cxx_std_17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(sudoku PRIVATE stdc++fs)
endif()
