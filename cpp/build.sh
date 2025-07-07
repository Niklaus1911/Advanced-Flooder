#!/bin/bash
# Advanced Flooder C++ Build Script for Linux

echo "Building Advanced Flooder C++ Version for Linux..."
echo "This version includes full cross-platform functionality."

# Check if X11 development packages are installed
if ! pkg-config --exists x11 xtst; then
    echo "ERROR: X11 development packages not found!"
    echo "Please install them using:"
    echo "  Ubuntu/Debian: sudo apt-get install libx11-dev libxtst-dev"
    echo "  CentOS/RHEL:   sudo yum install libX11-devel libXtst-devel"
    echo "  Fedora:        sudo dnf install libX11-devel libXtst-devel"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Clean previous build
rm -rf *

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
echo "Executable: build/AdvancedFlooder"
echo ""
echo "Usage:"
echo "  ./build/AdvancedFlooder          # Run with command-line interface"
echo ""
echo "Note: This application requires X11 and may need to be run with appropriate permissions."
echo "      You may need to run 'xhost +local:' to allow X11 access."
echo ""