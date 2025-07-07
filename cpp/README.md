# Advanced Flooder - Cross Platform C++ Version

A cross-platform message flooding application written in C++. This application can send repeated messages to processes or windows on both Windows and Linux systems.

## Features

- **Cross-Platform Support**: Runs on both Windows and Linux
- **Multiple Interfaces**: 
  - Windows: GUI and CLI modes
  - Linux: Command-line interface
- **Process Targeting**: Send messages to specific processes or active windows
- **Configurable Parameters**: Adjustable message count, delay, and targeting options
- **Real-time Feedback**: Shows message count and status during operation

## Supported Platforms

- **Windows 10/11**: Full GUI and CLI support
- **Linux**: Command-line interface with X11 support

## Prerequisites

### Windows
- Windows 10 or later
- Visual Studio 2019 or later (with C++ support)
- CMake 3.10 or later

### Linux
- Ubuntu 20.04 or later (or equivalent)
- GCC 9.0 or later
- CMake 3.10 or later
- X11 development packages:
  ```bash
  sudo apt-get install libx11-dev libxtst-dev
  ```

## Building

### Windows
1. Open a Command Prompt or PowerShell as Administrator
2. Navigate to the `cpp` directory
3. Run the build script:
   ```cmd
   build.bat
   ```

### Linux
1. Open a terminal
2. Navigate to the `cpp` directory
3. Run the build script:
   ```bash
   ./build.sh
   ```

## Usage

### Windows GUI Mode (Default)
```cmd
AdvancedFlooder.exe
```

### Windows CLI Mode
```cmd
AdvancedFlooder.exe -console
# or
AdvancedFlooder_CLI.exe
```

### Linux CLI Mode
```bash
./AdvancedFlooder
```

## Command Line Interface

The CLI provides the following options:

1. **Start flooding**: Configure and start message flooding
2. **List processes with windows**: Show all processes that have visible windows
3. **Test message send**: Send a single test message
4. **Show status**: Display current flooding status
5. **Stop flooding**: Stop ongoing message flooding
6. **Exit**: Quit the application

## Configuration Options

- **Message Text**: The text to send repeatedly
- **Maximum Messages**: Number of messages to send (0 for unlimited)
- **Delay**: Time between messages in milliseconds (minimum 50ms)
- **Target Type**: 
  - Active window: Send to currently focused window
  - Specific process: Send to a named process

## Safety Features

- **Minimum Delay**: Enforced 50ms minimum delay between messages
- **Process Validation**: Checks if target processes exist before sending
- **Interrupt Handling**: Ctrl+C support for emergency stop
- **Error Handling**: Graceful handling of missing windows or processes

## Technical Details

### Architecture
- **Cross-Platform Interface**: Abstract base classes for platform-specific implementations
- **Windows Implementation**: Uses Windows API (SendInput, Process32First, etc.)
- **Linux Implementation**: Uses X11 for window management and /proc filesystem for process enumeration

### Dependencies
- **Windows**: Windows SDK, Common Controls
- **Linux**: X11, XTest extension, pthread

### Key Components
- `CrossPlatform.h`: Abstract interfaces for cross-platform functionality
- `CrossPlatformWindows.cpp`: Windows-specific implementations
- `CrossPlatformLinux.cpp`: Linux-specific implementations
- `main_new.cpp`: Main application with both GUI and CLI support

## Known Limitations

- **Linux GUI**: Currently only supports CLI mode on Linux
- **X11 Dependency**: Linux version requires X11 windowing system
- **Permissions**: May require appropriate permissions for input simulation
- **Focus Requirements**: Target applications must be focusable for reliable message delivery

## Building from Source

### Windows Development
1. Install Visual Studio 2019 or later with C++ support
2. Install CMake 3.10 or later
3. Clone the repository
4. Run the build script or use CMake directly

### Linux Development
1. Install required packages:
   ```bash
   sudo apt-get install build-essential cmake libx11-dev libxtst-dev
   ```
2. Clone the repository
3. Run the build script or use CMake directly

## Troubleshooting

### Linux X11 Issues
If you encounter X11 permission errors:
```bash
xhost +local:
```

### Missing Dependencies
Ensure all development packages are installed:
```bash
# Ubuntu/Debian
sudo apt-get install libx11-dev libxtst-dev

# CentOS/RHEL
sudo yum install libX11-devel libXtst-devel

# Fedora
sudo dnf install libX11-devel libXtst-devel
```

### Build Issues
- Ensure CMake version is 3.10 or later
- Check that all required development packages are installed
- Verify compiler compatibility (GCC 9.0+ or Visual Studio 2019+)

## License

This project is developed by Niklaus1911. All rights reserved.
Website: www.igdownloader.com

## Disclaimer

This software is intended for educational and testing purposes only. Users are responsible for ensuring they have permission to send messages to target applications and comply with all applicable laws and regulations.

## Contributing

This is a personal project by Niklaus1911. For issues or suggestions, please contact through the official channels.

## Version History

- **v2.0**: Cross-platform C++ implementation with Linux support
- **v1.0**: Original Windows-only C# implementation