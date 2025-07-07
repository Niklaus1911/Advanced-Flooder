#ifdef _WIN32
#include "CrossPlatform.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <iostream>

class WindowsMessageSender : public ICrossPlatformMessageSender {
public:
    bool SendMessageToActiveWindow(const std::string& message) override {
        HWND hWnd = GetForegroundWindow();
        if (!hWnd) {
            return false;
        }
        
        SetForegroundWindow(hWnd);
        ::Sleep(50);
        
        std::wstring wideMessage = StringToWString(message);
        SendText(wideMessage);
        
        return true;
    }
    
    bool SendMessageToProcess(const std::string& processName, const std::string& message) override {
        HWND hWnd = FindWindowByProcess(processName);
        if (!hWnd) {
            return false;
        }
        
        if (!SetForegroundWindow(hWnd)) {
            return false;
        }
        
        ::Sleep(50);
        
        std::wstring wideMessage = StringToWString(message);
        SendText(wideMessage);
        
        return true;
    }
    
    void SendEnterKey() override {
        SendKey(VK_RETURN);
    }
    
    void Sleep(int milliseconds) override {
        ::Sleep(milliseconds);
    }
    
private:
    std::wstring StringToWString(const std::string& str) {
        if (str.empty()) {
            return std::wstring();
        }
        
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
        
        return wstr;
    }
    
    void SendText(const std::wstring& text) {
        std::vector<INPUT> inputs;
        inputs.reserve(text.length() * 2);
        
        for (wchar_t ch : text) {
            INPUT input = {0};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = 0;
            input.ki.wScan = ch;
            input.ki.dwFlags = KEYEVENTF_UNICODE;
            inputs.push_back(input);
            
            input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            inputs.push_back(input);
        }
        
        if (!inputs.empty()) {
            SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
        }
    }
    
    void SendKey(WORD vkCode) {
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vkCode;
        input.ki.dwFlags = 0;
        
        SendInput(1, &input, sizeof(INPUT));
        
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
    
    HWND FindWindowByProcess(const std::string& processName) {
        HWND hWnd = nullptr;
        
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return nullptr;
        }
        
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(hSnapshot, &pe32)) {
            do {
                std::string currentProcessName = pe32.szExeFile;
                
                std::transform(currentProcessName.begin(), currentProcessName.end(), 
                             currentProcessName.begin(), ::tolower);
                std::string searchName = processName;
                std::transform(searchName.begin(), searchName.end(), 
                             searchName.begin(), ::tolower);
                
                if (currentProcessName.find(searchName) != std::string::npos) {
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
                                return FALSE;
                            }
                        }
                        return TRUE;
                    }, reinterpret_cast<LPARAM>(&data));
                    
                    if (data.hWnd) {
                        hWnd = data.hWnd;
                        break;
                    }
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        return hWnd;
    }
};

class WindowsProcessEnumerator : public ICrossPlatformProcessEnumerator {
public:
    std::vector<ProcessInfo> GetProcesses() override {
        std::vector<ProcessInfo> processes;
        
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
                info.hasWindow = false;
                
                // Find the main window for this process
                struct EnumData {
                    DWORD processId;
                    HWND hWnd;
                    std::string windowTitle;
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
                            data->windowTitle = windowTitle;
                            return FALSE;
                        }
                    }
                    return TRUE;
                }, reinterpret_cast<LPARAM>(&data));
                
                if (data.hWnd) {
                    info.hasWindow = true;
                    info.windowTitle = data.windowTitle;
                }
                
                processes.push_back(info);
                
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        
        std::sort(processes.begin(), processes.end(), 
                  [](const ProcessInfo& a, const ProcessInfo& b) {
                      return a.name < b.name;
                  });
        
        return processes;
    }
    
    std::vector<ProcessInfo> GetProcessesWithWindows() override {
        std::vector<ProcessInfo> allProcesses = GetProcesses();
        std::vector<ProcessInfo> processesWithWindows;
        
        for (const auto& process : allProcesses) {
            if (process.hasWindow) {
                processesWithWindows.push_back(process);
            }
        }
        
        return processesWithWindows;
    }
    
    ProcessInfo FindProcess(const std::string& processName) override {
        std::vector<ProcessInfo> processes = GetProcesses();
        
        for (const auto& process : processes) {
            std::string currentName = process.name;
            std::string searchName = processName;
            
            std::transform(currentName.begin(), currentName.end(), 
                         currentName.begin(), ::tolower);
            std::transform(searchName.begin(), searchName.end(), 
                         searchName.begin(), ::tolower);
            
            if (currentName.find(searchName) != std::string::npos) {
                return process;
            }
        }
        
        ProcessInfo empty;
        empty.name = "";
        empty.processId = 0;
        empty.hasWindow = false;
        return empty;
    }
    
    bool ProcessExists(const std::string& processName) override {
        ProcessInfo info = FindProcess(processName);
        return !info.name.empty();
    }
};

ICrossPlatformMessageSender* CreateMessageSender() {
    return new WindowsMessageSender();
}

ICrossPlatformProcessEnumerator* CreateProcessEnumerator() {
    return new WindowsProcessEnumerator();
}

#endif // _WIN32