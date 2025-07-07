#include "CrossPlatform.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include "MainWindow.h"
#include "../resources/resource.h"
#else
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#endif

class AdvancedFlooder {
private:
    std::unique_ptr<ICrossPlatformMessageSender> messageSender;
    std::unique_ptr<ICrossPlatformProcessEnumerator> processEnumerator;
    std::atomic<bool> isRunning;
    std::atomic<bool> shouldStop;
    std::atomic<int> messageCount;
    
public:
    AdvancedFlooder() : isRunning(false), shouldStop(false), messageCount(0) {
        messageSender.reset(CreateMessageSender());
        processEnumerator.reset(CreateProcessEnumerator());
    }
    
    ~AdvancedFlooder() = default;
    
    void ShowHeader() {
        std::cout << "\n=== Advanced Flooder - Cross Platform Version ===" << std::endl;
        std::cout << "Author: Niklaus1911" << std::endl;
        std::cout << "Website: www.igdownloader.com" << std::endl;
        std::cout << "Platform: " << 
#ifdef _WIN32
        "Windows"
#else
        "Linux"
#endif
        << std::endl;
        std::cout << "=================================================" << std::endl;
    }
    
    void ShowMenu() {
        std::cout << "\nAvailable commands:" << std::endl;
        std::cout << "1. Start flooding" << std::endl;
        std::cout << "2. List processes with windows" << std::endl;
        std::cout << "3. Test message send" << std::endl;
        std::cout << "4. Show status" << std::endl;
        std::cout << "5. Stop flooding" << std::endl;
        std::cout << "6. Exit" << std::endl;
        std::cout << "Enter command (1-6): ";
    }
    
    void ListProcesses() {
        std::cout << "\nScanning for processes with windows..." << std::endl;
        std::vector<ProcessInfo> processes = processEnumerator->GetProcessesWithWindows();
        
        if (processes.empty()) {
            std::cout << "No processes with windows found." << std::endl;
            return;
        }
        
        std::cout << "\nProcesses with windows:" << std::endl;
        std::cout << std::left << std::setw(20) << "Process Name" 
                  << std::setw(10) << "PID" 
                  << "Window Title" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (const auto& process : processes) {
            std::cout << std::left << std::setw(20) << process.name
                      << std::setw(10) << process.processId
                      << process.windowTitle << std::endl;
        }
    }
    
    void TestMessage() {
        std::cout << "\nTest Message Send" << std::endl;
        std::cout << "1. Send to active window" << std::endl;
        std::cout << "2. Send to specific process" << std::endl;
        std::cout << "Enter choice (1-2): ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        std::string message;
        std::cout << "Enter message to send: ";
        std::getline(std::cin, message);
        
        if (message.empty()) {
            std::cout << "Message cannot be empty!" << std::endl;
            return;
        }
        
        bool success = false;
        
        if (choice == 1) {
            std::cout << "Sending message to active window in 3 seconds..." << std::endl;
            std::cout << "Switch to target window now!" << std::endl;
            
            for (int i = 3; i > 0; i--) {
                std::cout << i << "... ";
                std::cout.flush();
                messageSender->Sleep(1000);
            }
            std::cout << "GO!" << std::endl;
            
            success = messageSender->SendMessageToActiveWindow(message);
            if (success) {
                messageSender->SendEnterKey();
            }
        } else if (choice == 2) {
            std::string processName;
            std::cout << "Enter process name (e.g., notepad, firefox): ";
            std::getline(std::cin, processName);
            
            if (processName.empty()) {
                std::cout << "Process name cannot be empty!" << std::endl;
                return;
            }
            
            std::cout << "Sending message to process '" << processName << "'..." << std::endl;
            success = messageSender->SendMessageToProcess(processName, message);
            if (success) {
                messageSender->SendEnterKey();
            }
        }
        
        if (success) {
            std::cout << "Message sent successfully!" << std::endl;
        } else {
            std::cout << "Failed to send message!" << std::endl;
        }
    }
    
    void StartFlooding() {
        if (isRunning.load()) {
            std::cout << "Flooding is already running!" << std::endl;
            return;
        }
        
        std::string message;
        std::cout << "Enter message to flood: ";
        std::getline(std::cin, message);
        
        if (message.empty()) {
            std::cout << "Message cannot be empty!" << std::endl;
            return;
        }
        
        int maxMessages;
        std::cout << "Enter maximum number of messages (0 for unlimited): ";
        std::cin >> maxMessages;
        
        int delay;
        std::cout << "Enter delay between messages in milliseconds (minimum 50): ";
        std::cin >> delay;
        
        if (delay < 50) {
            std::cout << "Delay too short! Using minimum of 50ms." << std::endl;
            delay = 50;
        }
        
        std::cout << "Target:" << std::endl;
        std::cout << "1. Active window" << std::endl;
        std::cout << "2. Specific process" << std::endl;
        std::cout << "Enter choice (1-2): ";
        
        int targetChoice;
        std::cin >> targetChoice;
        std::cin.ignore(); // Clear the newline
        
        std::string processName;
        if (targetChoice == 2) {
            std::cout << "Enter process name: ";
            std::getline(std::cin, processName);
            
            if (processName.empty()) {
                std::cout << "Process name cannot be empty!" << std::endl;
                return;
            }
        }
        
        // Start flooding
        shouldStop.store(false);
        isRunning.store(true);
        messageCount.store(0);
        
        std::cout << "\nStarting flood in 3 seconds..." << std::endl;
        if (targetChoice == 1) {
            std::cout << "Switch to target window now!" << std::endl;
        }
        std::cout << "Press Ctrl+C to stop early." << std::endl;
        
        for (int i = 3; i > 0; i--) {
            std::cout << i << "... ";
            std::cout.flush();
            messageSender->Sleep(1000);
        }
        std::cout << "GO!" << std::endl;
        
        // Flooding loop
        int currentCount = 0;
        while (!shouldStop.load() && (maxMessages == 0 || currentCount < maxMessages)) {
            bool success = false;
            
            if (targetChoice == 1) {
                success = messageSender->SendMessageToActiveWindow(message);
            } else {
                success = messageSender->SendMessageToProcess(processName, message);
            }
            
            if (success) {
                messageSender->SendEnterKey();
                currentCount++;
                messageCount.store(currentCount);
                
                if (currentCount % 10 == 0) {
                    std::cout << "Sent " << currentCount << " messages..." << std::endl;
                }
            } else {
                std::cout << "Failed to send message (attempt " << currentCount + 1 << ")" << std::endl;
            }
            
            messageSender->Sleep(delay);
        }
        
        isRunning.store(false);
        std::cout << "\nFlooding stopped. Total messages sent: " << currentCount << std::endl;
    }
    
    void StopFlooding() {
        if (!isRunning.load()) {
            std::cout << "Flooding is not running!" << std::endl;
            return;
        }
        
        shouldStop.store(true);
        std::cout << "Stopping flood..." << std::endl;
    }
    
    void ShowStatus() {
        std::cout << "\nStatus:" << std::endl;
        std::cout << "Running: " << (isRunning.load() ? "Yes" : "No") << std::endl;
        std::cout << "Messages sent: " << messageCount.load() << std::endl;
    }
    
    void RunCLI() {
        ShowHeader();
        
        // Setup signal handler for Ctrl+C
#ifndef _WIN32
        signal(SIGINT, [](int) {
            std::cout << "\nReceived interrupt signal. Stopping..." << std::endl;
            exit(0);
        });
#endif
        
        std::string input;
        int choice;
        
        while (true) {
            ShowMenu();
            std::cin >> choice;
            std::cin.ignore(); // Clear the newline
            
            switch (choice) {
                case 1:
                    StartFlooding();
                    break;
                case 2:
                    ListProcesses();
                    break;
                case 3:
                    TestMessage();
                    break;
                case 4:
                    ShowStatus();
                    break;
                case 5:
                    StopFlooding();
                    break;
                case 6:
                    std::cout << "Exiting..." << std::endl;
                    if (isRunning.load()) {
                        shouldStop.store(true);
                    }
                    return;
                default:
                    std::cout << "Invalid choice! Please enter 1-6." << std::endl;
                    break;
            }
        }
    }
};

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Check if we should run in console mode
    bool runConsole = false;
    
    // Check command line arguments
    if (lpCmdLine && strlen(lpCmdLine) > 0) {
        std::string cmdLine(lpCmdLine);
        if (cmdLine.find("-console") != std::string::npos || cmdLine.find("-cli") != std::string::npos) {
            runConsole = true;
        }
    }
    
    // Allocate console if running in console mode
    if (runConsole) {
        AllocConsole();
        freopen_s(nullptr, "CONOUT$", "w", stdout);
        freopen_s(nullptr, "CONIN$", "r", stdin);
        freopen_s(nullptr, "CONOUT$", "w", stderr);
        
        AdvancedFlooder flooder;
        flooder.RunCLI();
        
        FreeConsole();
        return 0;
    }
    
    // Otherwise run GUI version
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    MainWindow window(hInstance);
    
    if (!window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    window.Show();
    window.RunMessageLoop();
    
    return 0;
}
#endif

int main(int /*argc*/, char* /*argv*/[]) {
    AdvancedFlooder flooder;
    flooder.RunCLI();
    return 0;
}