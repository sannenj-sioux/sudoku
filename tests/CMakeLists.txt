# CMakeLists.txt for the tests directory

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Use the parent project's runtime library settings on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

if(MSVC)
    foreach(_gtest_target gtest gtest_main gmock gmock_main)
        if(TARGET ${_gtest_target})
            get_target_property(_opts ${_gtest_target} COMPILE_OPTIONS)
            if(_opts)
                list(REMOVE_ITEM _opts "/source-charset:utf-8" "/execution-charset:utf-8")
                set_target_properties(${_gtest_target} PROPERTIES COMPILE_OPTIONS "${_opts}")
            endif()
        endif()
    endforeach()
endif()

include(GoogleTest)

# Manually specify source files
set(PERFORMANCE_TEST_FILES
    ../src/board.cpp
    ../src/block.cpp
    ../src/command.cpp
    ../src/i18n.cpp
    ../src/input.cpp
    ../src/puzzle_generator.cpp
    ../src/scene.cpp
    performance_test.cpp
)

add_executable(performance_test  # Output executable named performance_test
   ${PERFORMANCE_TEST_FILES}
)

target_compile_features(performance_test PRIVATE cxx_std_17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(performance_test PRIVATE stdc++fs)
endif()

set(UNIT_TEST_FILES
    ../src/board.cpp
    ../src/block.cpp
    ../src/command.cpp
    ../src/i18n.cpp
    ../src/input.cpp
    ../src/puzzle_generator.cpp
    ../src/scene.cpp
    puzzle_generation_test.cpp
    scene_test.cpp
)

add_executable(sudoku_unit_tests ${UNIT_TEST_FILES})
target_compile_features(sudoku_unit_tests PRIVATE cxx_std_17)
target_link_libraries(sudoku_unit_tests PRIVATE GTest::gmock GTest::gtest_main)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(sudoku_unit_tests PRIVATE stdc++fs)
endif()

gtest_discover_tests(sudoku_unit_tests
    EXTRA_ARGS --gtest_color=yes
)
