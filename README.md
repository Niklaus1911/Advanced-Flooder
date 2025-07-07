# Advanced Flooder

A powerful message automation application with both C# and C++ implementations.

## Project Structure

- **`Spammer/`** - Original C# version (Windows-only)
- **`cpp/`** - C++ port with cross-platform build support
- **`build.sh`** - Universal build script for all platforms

## Quick Start

### Universal Build (Recommended)

The universal build script automatically detects your platform and builds the appropriate version:

```bash
# Simple build
./build.sh

# Build with options
./build.sh --clean --debug --verbose

# Show help
./build.sh --help
```

### Platform-Specific Instructions

#### Linux/macOS
```bash
# Build C++ version (with stub implementations)
./build.sh --target cpp

# Or use the C++ specific script
cd cpp && ./build.sh
```

#### Windows
```bash
# Build C# version (full functionality)
./build.sh --target csharp

# Or build C++ version
./build.sh --target cpp

# Or use platform-specific scripts
cd cpp && build.bat
```

## Build Options

The universal build script supports these options:

- `--clean` - Clean build artifacts before building
- `--debug` - Build in debug mode (default: release)
- `--target TYPE` - Specify build target (cpp, csharp, auto)
- `--verbose` - Enable verbose output
- `--help` - Show help message

## Build Targets

### C++ Version (`cpp/`)
- **Platforms**: Linux, macOS, Windows
- **Dependencies**: CMake 3.10+, C++17 compiler
- **Features**: 
  - Cross-platform compilation
  - Stub implementations for non-Windows platforms
  - Native Windows functionality when built on Windows

### C# Version (`Spammer/`)
- **Platforms**: Windows (primary), Linux/macOS (with .NET)
- **Dependencies**: .NET Framework or .NET Core/5+
- **Features**: Full GUI functionality with Windows Forms

## Usage

### C++ Version
```bash
# After building
./cpp/build/AdvancedFlooder
```

### C# Version
```bash
# After building (Windows)
./Spammer/bin/Release/Spammer.exe
```

## Development

### C++ Build System
- Uses CMake for cross-platform builds
- Automatically detects platform and adjusts build configuration
- Provides stub implementations for Windows-specific APIs on non-Windows platforms

### Architecture
- **MainWindow**: UI and user interactions
- **MessageSender**: Message automation logic
- **ProcessEnumerator**: Process and window management

## Platform Notes

### Linux/macOS
The C++ version compiles and runs on Linux/macOS but with limited functionality due to the Windows-centric nature of the application. It serves as a compilation test and demonstrates the cross-platform build capability.

### Windows
Both C# and C++ versions provide full functionality on Windows, including:
- Native Windows GUI
- Process targeting
- Windows API integration
- Full message automation features

## Building from Source

### Prerequisites
- **Linux/macOS**: GCC/Clang, CMake 3.10+
- **Windows**: Visual Studio 2019+ or GCC, CMake 3.10+, optionally .NET SDK

### Step-by-step
1. Clone the repository
2. Run the universal build script: `./build.sh`
3. Follow any platform-specific instructions shown

## License

This project maintains the same license as the original implementation.

## Author

- Original C# version: Niklaus1911
- C++ port and build system: Automated conversion maintaining original functionality