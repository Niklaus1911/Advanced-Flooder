@echo off
REM Advanced Flooder C++ Build Script for Windows

echo Building Advanced Flooder C++ Version...

REM Create build directory
if not exist "build" mkdir build
cd build

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
echo Executable: build\Release\AdvancedFlooder.exe
echo.
pause