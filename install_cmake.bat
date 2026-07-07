@echo off
setlocal

echo [INFO] Installing CMake using winget...

where winget >nul 2>nul
if errorlevel 1 (
  echo [ERROR] winget was not found on this machine.
  echo [HINT] Install App Installer from Microsoft Store, then rerun this script.
  exit /b 1
)

winget install --id Kitware.CMake --accept-package-agreements --accept-source-agreements -e
if errorlevel 1 (
  echo [ERROR] CMake installation failed.
  exit /b 1
)

echo [INFO] Verifying CMake installation...
where cmake >nul 2>nul
if not errorlevel 1 (
  cmake --version
  echo [SUCCESS] CMake is installed and available on PATH.
  exit /b 0
)

if exist "C:\Program Files\CMake\bin\cmake.exe" (
  "C:\Program Files\CMake\bin\cmake.exe" --version
  echo [SUCCESS] CMake is installed at C:\Program Files\CMake\bin\cmake.exe
  echo [NOTE] Open a new terminal so PATH updates are applied.
  exit /b 0
)

echo [WARNING] CMake may be installed, but it was not found on PATH yet.
echo [HINT] Open a new terminal and run: cmake --version
exit /b 0
