#pragma once

#ifndef CROSSPLATFORM_H
#define CROSSPLATFORM_H

#include <string>
#include <vector>

// Cross-platform process information
struct ProcessInfo {
    std::string name;
    unsigned long processId;
    std::string windowTitle;
    bool hasWindow;
};

// Cross-platform interface for message sending
class ICrossPlatformMessageSender {
public:
    virtual ~ICrossPlatformMessageSender() = default;
    
    // Send message to the currently active window
    virtual bool SendMessageToActiveWindow(const std::string& message) = 0;
    
    // Send message to a specific process
    virtual bool SendMessageToProcess(const std::string& processName, const std::string& message) = 0;
    
    // Send Enter key
    virtual void SendEnterKey() = 0;
    
    // Sleep for specified milliseconds
    virtual void Sleep(int milliseconds) = 0;
};

// Cross-platform interface for process enumeration
class ICrossPlatformProcessEnumerator {
public:
    virtual ~ICrossPlatformProcessEnumerator() = default;
    
    // Get all running processes
    virtual std::vector<ProcessInfo> GetProcesses() = 0;
    
    // Get processes with windows
    virtual std::vector<ProcessInfo> GetProcessesWithWindows() = 0;
    
    // Find process by name
    virtual ProcessInfo FindProcess(const std::string& processName) = 0;
    
    // Check if process exists
    virtual bool ProcessExists(const std::string& processName) = 0;
};

// Factory functions to create platform-specific implementations
ICrossPlatformMessageSender* CreateMessageSender();
ICrossPlatformProcessEnumerator* CreateProcessEnumerator();

#endif // CROSSPLATFORM_H