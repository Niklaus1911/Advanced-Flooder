#pragma once

#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#ifdef COMPILE_TEST_ONLY
// Stub declarations for compilation testing
typedef void* HWND;
typedef unsigned short WORD;
#else
#include <windows.h>
#endif

#include <string>

class MessageSender {
public:
    MessageSender();
    ~MessageSender();
    
    // Send message to the foreground window
    bool SendMessage(const std::string& message);
    
    // Send message to a specific process
    bool SendMessageToProcess(const std::string& processName, const std::string& message);
    
    // Set focus to a window
    bool SetForegroundWindow(HWND hWnd);
    
    // Find window by process name
    HWND FindWindowByProcess(const std::string& processName);
    
    // Simulate key press
    void SendEnterKey();
    
private:
    // Convert string to wide string for Windows API
    std::wstring StringToWString(const std::string& str);
    
    // Send text using SendInput
    void SendText(const std::wstring& text);
    
    // Send a single key
    void SendKey(WORD vkCode);
};

#endif // MESSAGESENDER_H