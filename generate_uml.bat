@echo off
setlocal

set "ROOT=%~dp0"
pushd "%ROOT%" >nul

echo [INFO] Preparing to generate UML diagrams with clang-uml...

set "CMAKE_BIN=cmake"
where cmake >nul 2>nul
if errorlevel 1 (
  if exist "C:\Program Files\CMake\bin\cmake.exe" (
    set "CMAKE_BIN=C:\Program Files\CMake\bin\cmake.exe"
  ) else (
    echo [ERROR] CMake was not found.
    echo [HINT] Run install_cmake.bat first.
    goto :fail
  )
)

set "CLANG_UML_BIN=clang-uml"
where clang-uml >nul 2>nul
if errorlevel 1 (
  if exist "C:\Program Files\clang-uml\bin\clang-uml.exe" (
    set "CLANG_UML_BIN=C:\Program Files\clang-uml\bin\clang-uml.exe"
  ) else (
    echo [ERROR] clang-uml was not found on PATH.
    echo [HINT] Run install_clang_uml.bat and open a new terminal.
    goto :fail
  )
)

set "NINJA_BIN=ninja"
where ninja >nul 2>nul
if errorlevel 1 (
  if exist "%LOCALAPPDATA%\Microsoft\WinGet\Links\ninja.exe" (
    set "NINJA_BIN=%LOCALAPPDATA%\Microsoft\WinGet\Links\ninja.exe"
  ) else (
    echo [ERROR] ninja was not found on PATH.
    echo [HINT] Run install_clang_uml.bat and open a new terminal.
    goto :fail
  )
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo [ERROR] vswhere.exe not found. Cannot locate Visual Studio Build Tools.
  goto :fail
)

set "VSINSTALL="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"

if "%VSINSTALL%"=="" (
  echo [ERROR] Visual Studio C++ build tools were not found.
  goto :fail
)

set "VSDEVCMD=%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" (
  echo [ERROR] VsDevCmd.bat not found at "%VSDEVCMD%".
  goto :fail
)

echo [INFO] Configuring build-uml with Ninja to emit compile_commands.json...
set "PATH=%LOCALAPPDATA%\Microsoft\WinGet\Links;%PATH%"
call "%VSDEVCMD%" -arch=x64 -host_arch=x64
"%CMAKE_BIN%" -S . -B build-uml -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_FLAGS=/EHsc
if errorlevel 1 (
  echo [ERROR] CMake configure failed.
  goto :fail
)

if not exist "build-uml\compile_commands.json" (
  echo [ERROR] compile_commands.json was not generated.
  goto :fail
)

if not exist "uml" mkdir "uml"

echo [INFO] Generating diagrams...
"%CLANG_UML_BIN%" --config .clang-uml
if errorlevel 1 (
  echo [ERROR] clang-uml generation failed.
  goto :fail
)

echo [SUCCESS] UML generation complete.
echo [INFO] Output folder: %ROOT%uml
popd >nul
exit /b 0

:fail
popd >nul
exit /b 1
