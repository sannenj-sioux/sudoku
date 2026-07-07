@echo off
setlocal

echo [INFO] Installing clang-uml and Ninja via winget...

where winget >nul 2>nul
if errorlevel 1 (
  echo [ERROR] winget was not found on this machine.
  echo [HINT] Install App Installer from Microsoft Store, then rerun this script.
  exit /b 1
)

winget install --id bkryza.clang-uml --accept-package-agreements --accept-source-agreements -e
if errorlevel 1 (
  echo [ERROR] Failed to install clang-uml.
  exit /b 1
)

winget install --id Ninja-build.Ninja --accept-package-agreements --accept-source-agreements -e
if errorlevel 1 (
  echo [ERROR] Failed to install Ninja.
  exit /b 1
)

echo [INFO] Verifying tools...
set "CLANG_UML_BIN=clang-uml"
where clang-uml >nul 2>nul
if errorlevel 1 (
  if exist "C:\Program Files\clang-uml\bin\clang-uml.exe" (
    set "CLANG_UML_BIN=C:\Program Files\clang-uml\bin\clang-uml.exe"
  ) else (
    echo [WARNING] clang-uml is installed but not visible in this terminal PATH yet.
    echo [HINT] Open a new terminal, then run: clang-uml --version
  )
)

if exist "%CLANG_UML_BIN%" (
  "%CLANG_UML_BIN%" --version
)

set "NINJA_BIN=ninja"
where ninja >nul 2>nul
if errorlevel 1 (
  if exist "%LOCALAPPDATA%\Microsoft\WinGet\Links\ninja.exe" (
    set "NINJA_BIN=%LOCALAPPDATA%\Microsoft\WinGet\Links\ninja.exe"
  ) else (
    echo [WARNING] ninja is installed but not visible in this terminal PATH yet.
    echo [HINT] Open a new terminal, then run: ninja --version
  )
)

if exist "%NINJA_BIN%" (
  "%NINJA_BIN%" --version
)

echo [SUCCESS] Installation script completed.
exit /b 0
