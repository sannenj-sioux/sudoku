CMAKE_MINIMUM_REQUIRED(VERSION 3.12) # cmake最低版本要求

PROJECT(sudoku)    # 工程名 CppTemplate

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(SUDOKU_SKIP_COMPILE_DB_REFRESH "Skip nested compile_commands refresh target" OFF)

set(CMAKE_CXX_STANDARD 17) # C++17标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_BUILD_TYPE "Debug")  # Debug模式 选项: Release Debug MinSizeRel RelWithDebInfo
# 针对不同模式设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if (MSVC)
    add_compile_options(/W4 /Od)
  else()
    add_compile_options(-O0 -Wall -g2)
  endif()
else(CMAKE_BUILD_TYPE STREQUAL "Release")
  if (MSVC)
    add_compile_options(/W4 /O2)
  else()
    add_compile_options(-O3 -Wall)
  endif()
endif()

# set(CMAKE_VERBOSE_MAKEFILE ON) # 开启调试 出现问题时开启

include(CTest)

add_definitions(-DDEBUG) # 相当于代码中 #define DEBUG

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin") # 可执行文件输出目录

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/interfaces") # 头文件包含目录

# 这段代码可以区分操作系统
MESSAGE("Identifying the OS...")
if(WIN32)
  MESSAGE("This is Windows.")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/source-charset:utf-8")
    add_compile_options("/execution-charset:utf-8")
  else()
    add_compile_options("-finput-charset=utf-8")
    add_compile_options("-fexec-charset=utf-8")
  endif()
elseif(APPLE)
  MESSAGE("This is MacOS.")
elseif(UNIX)
  MESSAGE("This is Linux.")
endif()
# 这段代码可以区分操作系统

# 添加子目录
add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Visual Studio generators don't emit compile_commands.json directly.
# Keep a CMake-native refresh target in ALL so clangd DB stays updated with normal builds.
if(CMAKE_GENERATOR MATCHES "Visual Studio" AND NOT SUDOKU_SKIP_COMPILE_DB_REFRESH)
  add_custom_target(refresh_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -S ${PROJECT_SOURCE_DIR} -B ${PROJECT_BINARY_DIR}/clangd-db -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DSUDOKU_SKIP_COMPILE_DB_REFRESH=ON
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_BINARY_DIR}/clangd-db/compile_commands.json ${PROJECT_BINARY_DIR}/compile_commands.json
    COMMENT "Refreshing compile_commands.json for clangd"
    VERBATIM
  )
endif()

# UML generation via clang-uml (manual build target).
find_program(CLANG_UML_EXE
  NAMES clang-uml clang-uml.exe
  HINTS
    "C:/Program Files/clang-uml/bin"
    "$ENV{LOCALAPPDATA}/Microsoft/WinGet/Links"
)

if(CLANG_UML_EXE)
  add_custom_target(generate_uml
    COMMAND ${CLANG_UML_EXE} --config ${PROJECT_SOURCE_DIR}/.clang-uml
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating UML diagrams with clang-uml"
    VERBATIM
  )

  if(TARGET refresh_compile_commands)
    add_dependencies(generate_uml refresh_compile_commands)
  endif()
else()
  add_custom_target(generate_uml
    COMMAND ${CMAKE_COMMAND} -E echo "clang-uml was not found. Install it first (see install_clang_uml.bat)."
    COMMAND ${CMAKE_COMMAND} -E false
    COMMENT "Cannot generate UML because clang-uml is missing"
    VERBATIM
  )
endif()

# Code coverage generation via OpenCppCoverage (manual build target, Windows).
if(WIN32)
  string(REPLACE "/" "\\" PROJECT_SOURCE_DIR_WIN "${PROJECT_SOURCE_DIR}")

  find_program(OPENCPPCOVERAGE_EXE
    NAMES OpenCppCoverage OpenCppCoverage.exe
    HINTS
      "C:/Program Files/OpenCppCoverage"
      "$ENV{LOCALAPPDATA}/Microsoft/WinGet/Links"
  )

  if(OPENCPPCOVERAGE_EXE)
    add_custom_target(generate_coverage
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/coverage/html
      COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/coverage/html
      COMMAND ${OPENCPPCOVERAGE_EXE}
        --cover_children
        --modules "${PROJECT_SOURCE_DIR_WIN}\\bin\\Debug\\*"
        --sources "${PROJECT_SOURCE_DIR_WIN}\\src\\*"
        --excluded_sources "*tests*"
        --excluded_sources "*build*"
        --export_type "html:${PROJECT_SOURCE_DIR}/coverage/html"
        --
        ${CMAKE_CTEST_COMMAND} --test-dir ${PROJECT_BINARY_DIR} -C Debug --output-on-failure
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMENT "Generating HTML code coverage report with OpenCppCoverage"
      VERBATIM
    )

    if(TARGET sudoku_unit_tests)
      add_dependencies(generate_coverage sudoku_unit_tests)
    endif()
  else()
    add_custom_target(generate_coverage
      COMMAND ${CMAKE_COMMAND} -E echo "OpenCppCoverage was not found. Install it first (see install_opencppcoverage.bat)."
      COMMAND ${CMAKE_COMMAND} -E false
      COMMENT "Cannot generate coverage because OpenCppCoverage is missing"
      VERBATIM
    )
  endif()
else()
  add_custom_target(generate_coverage
    COMMAND ${CMAKE_COMMAND} -E echo "generate_coverage is currently supported on Windows only."
    COMMAND ${CMAKE_COMMAND} -E false
    COMMENT "Coverage target is unavailable on this platform"
    VERBATIM
  )
endif()
