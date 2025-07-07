@echo off
REM Advanced Flooder C++ Build Script for Windows

echo Building Advanced Flooder C++ Version for Windows...
echo This version includes both GUI and CLI interfaces.

REM Create build directory
if not exist "build" mkdir build
cd build

REM Clean previous build
del /Q *.*

REM Generate build files
cmake ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo GUI Executable: build\Release\AdvancedFlooder.exe
echo CLI Executable: build\Release\AdvancedFlooder_CLI.exe
echo.
echo Usage:
echo   AdvancedFlooder.exe                # Run with GUI interface
echo   AdvancedFlooder.exe -console       # Run with CLI interface
echo   AdvancedFlooder_CLI.exe            # Run CLI-only version
echo.
pause