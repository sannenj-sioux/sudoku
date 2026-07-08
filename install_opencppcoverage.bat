@echo off
setlocal

echo [INFO] Installing OpenCppCoverage using winget...

where winget >nul 2>nul
if errorlevel 1 (
  echo [ERROR] winget was not found on this machine.
  echo [HINT] Install App Installer from Microsoft Store, then rerun this script.
  exit /b 1
)

winget install --id OpenCppCoverage.OpenCppCoverage --accept-package-agreements --accept-source-agreements -e
if errorlevel 1 (
  echo [ERROR] OpenCppCoverage installation failed.
  exit /b 1
)

echo [INFO] Verifying OpenCppCoverage installation...
where OpenCppCoverage >nul 2>nul
if not errorlevel 1 (
  OpenCppCoverage --version
  echo [SUCCESS] OpenCppCoverage is installed and available on PATH.
  exit /b 0
)

if exist "C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe" (
  "C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe" --version
  echo [SUCCESS] OpenCppCoverage is installed at C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe
  echo [NOTE] Open a new terminal so PATH updates are applied.
  exit /b 0
)

echo [WARNING] OpenCppCoverage may be installed, but it was not found on PATH yet.
echo [HINT] Open a new terminal and run: OpenCppCoverage --version
exit /b 0
