# Advanced Flooder - C++ Version

This is a C++ port of the Advanced Flooder application, originally written in C#. The application allows you to automatically send repeated messages to other applications with configurable delays.

## Features

- **Message Automation**: Send repeated messages with configurable delays
- **Process Targeting**: Option to target specific processes or use the foreground window
- **Configurable Settings**: 
  - Adjustable delay between messages (minimum 50ms)
  - Maximum message count
  - Custom message text
- **User Interface**: Native Windows dialog-based interface
- **Keyboard Shortcuts**: F5 to start, F6 to stop
- **Real-time Status**: Shows current message count and operation status

## Building

### Prerequisites

- Windows 10 or later
- Visual Studio 2019 or later with C++ support
- CMake 3.10 or later

### Build Instructions

1. Clone the repository
2. Navigate to the `cpp` directory
3. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```
4. Generate build files:
   ```bash
   cmake ..
   ```
5. Build the project:
   ```bash
   cmake --build . --config Release
   ```

### Alternative Build with Visual Studio

You can also open the project directly in Visual Studio 2019+ using the "Open Folder" feature and pointing to the `cpp` directory.

## Usage

1. Run the `AdvancedFlooder.exe` executable
2. Configure your settings:
   - **Message Text**: Enter the message you want to send repeatedly
   - **Max Messages**: Set the maximum number of messages to send
   - **Delay**: Set the delay between messages in milliseconds (minimum 50ms)
3. **Optional**: Enable "Use by process" to target a specific application
   - Select a process from the dropdown
   - Click "Reload" to refresh the process list
4. Click "Start Thread" to begin sending messages
5. Click "Stop Thread" to stop the operation
6. Use F5/F6 keyboard shortcuts for quick start/stop

## Technical Details

### Architecture

The C++ version maintains the same functionality as the original C# version with the following components:

- **MainWindow**: Handles the UI and user interactions
- **MessageSender**: Responsible for sending messages to target applications
- **ProcessEnumerator**: Enumerates running processes and their windows

### Win32 API Usage

The application uses several Win32 APIs:
- `SendInput` for simulating keyboard input
- `SetForegroundWindow` for bringing target windows to focus
- `EnumWindows` and `EnumProcesses` for process enumeration
- Dialog API for the user interface

### Threading

The application uses C++11 threading (`std::thread`) for background message sending operations, ensuring the UI remains responsive.

## Safety Notice

This application is intended for legitimate automation purposes. Please use responsibly and in accordance with the terms of service of the applications you're targeting. The authors are not responsible for any misuse of this software.

## Differences from C# Version

- Uses native Win32 dialogs instead of Windows Forms
- Implements custom process enumeration using Win32 APIs
- Uses `SendInput` instead of `SendKeys` for more reliable input simulation
- Simplified UI layout due to Win32 dialog limitations
- Better error handling and input validation

## License

This project maintains the same license as the original C# version.

## Author

Original C# version: Niklaus1911
C++ Port: Automated conversion maintaining original functionality