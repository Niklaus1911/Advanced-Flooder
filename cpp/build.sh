#!/bin/bash
# Advanced Flooder C++ Build Script for Linux/macOS (compilation testing only)

echo "Building Advanced Flooder C++ Version (Compilation Test)..."

# Create build directory
mkdir -p build
cd build

# Generate build files
cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Build completed successfully!"
echo "Note: This is a compilation test only. The application requires Windows to run properly."
echo "Test executable: build/AdvancedFlooder"
echo ""