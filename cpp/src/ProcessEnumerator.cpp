#ifdef COMPILE_TEST_ONLY
// Stub implementations for compilation testing
#include "ProcessEnumerator.h"
#include <iostream>

#define TRUE 1
#define FALSE 0

std::vector<ProcessInfo>* ProcessEnumerator::s_processes = nullptr;

ProcessEnumerator::ProcessEnumerator() {
    std::cout << "ProcessEnumerator created (stub)" << std::endl;
}

ProcessEnumerator::~ProcessEnumerator() {
    std::cout << "ProcessEnumerator destroyed (stub)" << std::endl;
}

std::vector<ProcessInfo> ProcessEnumerator::GetProcesses() { return {}; }
std::vector<ProcessInfo> ProcessEnumerator::GetProcessesWithWindows() { return {}; }
ProcessInfo ProcessEnumerator::FindProcess(const std::string& processName) { return {}; }
bool ProcessEnumerator::ProcessExists(const std::string& processName) { return false; }
BOOL CALLBACK ProcessEnumerator::EnumWindowsProc(HWND hWnd, LPARAM lParam) { return TRUE; }
std::string ProcessEnumerator::GetProcessNameFromWindow(HWND hWnd) { return ""; }
bool ProcessEnumerator::IsValidWindow(HWND hWnd) { return false; }
std::string ProcessEnumerator::GetWindowTitle(HWND hWnd) { return ""; }
std::string ProcessEnumerator::WStringToString(const std::wstring& wstr) { return ""; }

#else
#include "ProcessEnumerator.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <algorithm>

// Static member initialization
std::vector<ProcessInfo>* ProcessEnumerator::s_processes = nullptr;

ProcessEnumerator::ProcessEnumerator() {
}

ProcessEnumerator::~ProcessEnumerator() {
}

std::vector<ProcessInfo> ProcessEnumerator::GetProcesses() {
    std::vector<ProcessInfo> processes;
    
    // Create a snapshot of all processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.name = pe32.szExeFile;
            info.processId = pe32.th32ProcessID;
            info.mainWindow = nullptr;
            
            // Find the main window for this process
            struct EnumData {
                DWORD processId;
                HWND hWnd;
            };
            
            EnumData data;
            data.processId = pe32.th32ProcessID;
            data.hWnd = nullptr;
            
            EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
                EnumData* data = reinterpret_cast<EnumData*>(lParam);
                DWORD processId;
                GetWindowThreadProcessId(hWnd, &processId);
                
                if (processId == data->processId && IsWindowVisible(hWnd)) {
                    char windowTitle[256];
                    if (GetWindowTextA(hWnd, windowTitle, sizeof(windowTitle)) > 0) {
                        data->hWnd = hWnd;
                        return FALSE; // Stop enumeration
                    }
                }
                return TRUE; // Continue enumeration
            }, reinterpret_cast<LPARAM>(&data));
            
            info.mainWindow = data.hWnd;
            processes.push_back(info);
            
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    
    // Sort by name
    std::sort(processes.begin(), processes.end(), 
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.name < b.name;
              });
    
    return processes;
}

std::vector<ProcessInfo> ProcessEnumerator::GetProcessesWithWindows() {
    std::vector<ProcessInfo> allProcesses = GetProcesses();
    std::vector<ProcessInfo> processesWithWindows;
    
    for (const auto& process : allProcesses) {
        if (process.mainWindow != nullptr) {
            processesWithWindows.push_back(process);
        }
    }
    
    return processesWithWindows;
}

ProcessInfo ProcessEnumerator::FindProcess(const std::string& processName) {
    std::vector<ProcessInfo> processes = GetProcesses();
    
    for (const auto& process : processes) {
        std::string currentName = process.name;
        std::string searchName = processName;
        
        // Case-insensitive comparison
        std::transform(currentName.begin(), currentName.end(), 
                     currentName.begin(), ::tolower);
        std::transform(searchName.begin(), searchName.end(), 
                     searchName.begin(), ::tolower);
        
        if (currentName.find(searchName) != std::string::npos) {
            return process;
        }
    }
    
    // Return empty ProcessInfo if not found
    ProcessInfo empty;
    empty.name = "";
    empty.processId = 0;
    empty.mainWindow = nullptr;
    return empty;
}

bool ProcessEnumerator::ProcessExists(const std::string& processName) {
    ProcessInfo info = FindProcess(processName);
    return !info.name.empty();
}

BOOL CALLBACK ProcessEnumerator::EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd)) {
        return TRUE;
    }
    
    char windowTitle[256];
    if (GetWindowTextA(hWnd, windowTitle, sizeof(windowTitle)) == 0) {
        return TRUE;
    }
    
    DWORD processId;
    GetWindowThreadProcessId(hWnd, &processId);
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return TRUE;
    }
    
    char processName[MAX_PATH];
    if (GetModuleBaseNameA(hProcess, nullptr, processName, sizeof(processName)) == 0) {
        CloseHandle(hProcess);
        return TRUE;
    }
    
    CloseHandle(hProcess);
    
    ProcessInfo info;
    info.name = processName;
    info.processId = processId;
    info.mainWindow = hWnd;
    
    if (s_processes) {
        s_processes->push_back(info);
    }
    
    return TRUE;
}

std::string ProcessEnumerator::GetProcessNameFromWindow(HWND hWnd) {
    DWORD processId;
    GetWindowThreadProcessId(hWnd, &processId);
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return "";
    }
    
    char processName[MAX_PATH];
    if (GetModuleBaseNameA(hProcess, nullptr, processName, sizeof(processName)) == 0) {
        CloseHandle(hProcess);
        return "";
    }
    
    CloseHandle(hProcess);
    return processName;
}

bool ProcessEnumerator::IsValidWindow(HWND hWnd) {
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd)) {
        return false;
    }
    
    char windowTitle[256];
    return GetWindowTextA(hWnd, windowTitle, sizeof(windowTitle)) > 0;
}

std::string ProcessEnumerator::GetWindowTitle(HWND hWnd) {
    char windowTitle[256];
    if (GetWindowTextA(hWnd, windowTitle, sizeof(windowTitle)) > 0) {
        return windowTitle;
    }
    return "";
}

std::string ProcessEnumerator::WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    
    return str;
}

#endif // COMPILE_TEST_ONLY