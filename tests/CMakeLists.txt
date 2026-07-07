# CMakeLists.txt for the tests directory

# Manually specify source files
set(PERFORMANCE_TEST_FILES
    ../src/block.cpp
    ../src/command.cpp
    ../src/i18n.cpp
    ../src/input.cpp
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
