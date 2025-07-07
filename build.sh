#!/bin/bash
# Advanced Flooder - Universal Build Script
# Builds the appropriate version based on the current platform

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to display help
show_help() {
    echo "Advanced Flooder - Universal Build Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -c, --clean         Clean build artifacts before building"
    echo "  -d, --debug         Build in debug mode (default: release)"
    echo "  -t, --target TYPE   Specify build target (cpp, csharp, auto)"
    echo "  -v, --verbose       Enable verbose output"
    echo ""
    echo "Build targets:"
    echo "  cpp                 Build C++ version (Linux/macOS/Windows)"
    echo "  csharp              Build C# version (Windows only)"
    echo "  auto                Auto-detect best target for platform (default)"
    echo ""
    echo "Examples:"
    echo "  $0                  Auto-build for current platform"
    echo "  $0 --target cpp     Build C++ version"
    echo "  $0 --clean --debug  Clean and build in debug mode"
    echo ""
}

# Default options
CLEAN=false
DEBUG=false
TARGET="auto"
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -d|--debug)
            DEBUG=true
            shift
            ;;
        -t|--target)
            TARGET="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Detect platform
detect_platform() {
    case "$(uname -s)" in
        Linux*)     PLATFORM=Linux;;
        Darwin*)    PLATFORM=macOS;;
        CYGWIN*)    PLATFORM=Windows;;
        MINGW*)     PLATFORM=Windows;;
        MSYS*)      PLATFORM=Windows;;
        *)          PLATFORM=Unknown;;
    esac
}

# Check if .NET is available for C# builds
check_dotnet() {
    if command -v dotnet &> /dev/null; then
        return 0
    elif command -v msbuild &> /dev/null; then
        return 0
    else
        return 1
    fi
}

# Build C++ version
build_cpp() {
    print_info "Building C++ version..."
    
    if [[ ! -d "cpp" ]]; then
        print_error "C++ source directory not found!"
        exit 1
    fi
    
    cd cpp
    
    # Clean if requested
    if [[ "$CLEAN" == true ]]; then
        print_info "Cleaning C++ build artifacts..."
        rm -rf build
    fi
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure CMake
    CMAKE_ARGS=".."
    if [[ "$DEBUG" == true ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug"
        print_info "Building in Debug mode..."
    else
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release"
        print_info "Building in Release mode..."
    fi
    
    if [[ "$VERBOSE" == true ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
    fi
    
    print_info "Running CMake configuration..."
    cmake $CMAKE_ARGS
    
    # Build
    print_info "Building project..."
    if command -v nproc &> /dev/null; then
        JOBS=$(nproc)
    elif command -v sysctl &> /dev/null; then
        JOBS=$(sysctl -n hw.ncpu)
    else
        JOBS=4
    fi
    
    if [[ "$VERBOSE" == true ]]; then
        make -j$JOBS VERBOSE=1
    else
        make -j$JOBS
    fi
    
    print_success "C++ build completed successfully!"
    
    if [[ "$PLATFORM" == "Linux" || "$PLATFORM" == "macOS" ]]; then
        print_info "Note: This is a compilation test build with stub implementations."
        print_info "The application is designed for Windows and uses Windows-specific APIs."
    fi
    
    print_info "Executable location: cpp/build/AdvancedFlooder"
    cd ../..
}

# Build C# version
build_csharp() {
    print_info "Building C# version..."
    
    if [[ ! -d "Spammer" ]]; then
        print_error "C# source directory not found!"
        exit 1
    fi
    
    if ! check_dotnet; then
        print_error "Neither dotnet nor msbuild found! Please install .NET SDK or Visual Studio."
        exit 1
    fi
    
    cd Spammer
    
    # Clean if requested
    if [[ "$CLEAN" == true ]]; then
        print_info "Cleaning C# build artifacts..."
        if command -v dotnet &> /dev/null; then
            dotnet clean
        fi
    fi
    
    # Build
    if command -v dotnet &> /dev/null; then
        BUILD_CONFIG="Release"
        if [[ "$DEBUG" == true ]]; then
            BUILD_CONFIG="Debug"
        fi
        
        print_info "Building with dotnet (Configuration: $BUILD_CONFIG)..."
        if [[ "$VERBOSE" == true ]]; then
            dotnet build -c $BUILD_CONFIG -v detailed
        else
            dotnet build -c $BUILD_CONFIG
        fi
    else
        print_info "Building with msbuild..."
        msbuild Spammer.csproj
    fi
    
    print_success "C# build completed successfully!"
    print_info "Executable location: Spammer/bin/Release/ or Spammer/bin/Debug/"
    cd ..
}

# Auto-detect best build target
auto_detect_target() {
    detect_platform
    
    case "$PLATFORM" in
        Linux|macOS)
            print_info "Detected platform: $PLATFORM"
            print_info "Auto-selecting C++ build (C# requires Windows)"
            build_cpp
            ;;
        Windows)
            print_info "Detected platform: $PLATFORM"
            if check_dotnet; then
                print_info "Auto-selecting C# build (.NET available)"
                build_csharp
            else
                print_info "Auto-selecting C++ build (.NET not available)"
                build_cpp
            fi
            ;;
        *)
            print_warning "Unknown platform: $PLATFORM"
            print_info "Defaulting to C++ build"
            build_cpp
            ;;
    esac
}

# Main execution
main() {
    echo "=============================================="
    echo "   Advanced Flooder - Universal Build Script"
    echo "=============================================="
    echo ""
    
    detect_platform
    print_info "Detected platform: $PLATFORM"
    
    case "$TARGET" in
        cpp)
            build_cpp
            ;;
        csharp)
            if [[ "$PLATFORM" != "Windows" ]]; then
                print_warning "C# build is primarily designed for Windows"
                print_info "Attempting to build anyway..."
            fi
            build_csharp
            ;;
        auto)
            auto_detect_target
            ;;
        *)
            print_error "Invalid target: $TARGET"
            print_error "Valid targets: cpp, csharp, auto"
            exit 1
            ;;
    esac
    
    echo ""
    print_success "Build process completed!"
    echo "=============================================="
}

# Run main function
main