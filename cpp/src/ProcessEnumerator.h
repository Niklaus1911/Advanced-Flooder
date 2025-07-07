#pragma once

#ifndef PROCESSENUMERATOR_H
#define PROCESSENUMERATOR_H

#ifdef COMPILE_TEST_ONLY
// Stub declarations for compilation testing
typedef void* HWND;
typedef unsigned long DWORD;
typedef long LPARAM;
typedef int BOOL;
#define CALLBACK
#else
#include <windows.h>
#endif

#include <string>
#include <vector>

struct ProcessInfo {
    std::string name;
    DWORD processId;
    HWND mainWindow;
};

class ProcessEnumerator {
public:
    ProcessEnumerator();
    ~ProcessEnumerator();
    
    // Get all running processes
    std::vector<ProcessInfo> GetProcesses();
    
    // Get processes with visible windows
    std::vector<ProcessInfo> GetProcessesWithWindows();
    
    // Find process by name
    ProcessInfo FindProcess(const std::string& processName);
    
    // Check if process exists
    bool ProcessExists(const std::string& processName);
    
private:
    // Enumerate windows callback
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
    
    // Get process name from window handle
    std::string GetProcessNameFromWindow(HWND hWnd);
    
    // Check if window is visible and has a title
    bool IsValidWindow(HWND hWnd);
    
    // Get window title
    std::string GetWindowTitle(HWND hWnd);
    
    // Convert wide string to string
    std::string WStringToString(const std::wstring& wstr);
    
    // Static vector for enumeration callback
    static std::vector<ProcessInfo>* s_processes;
};

#endif // PROCESSENUMERATOR_H