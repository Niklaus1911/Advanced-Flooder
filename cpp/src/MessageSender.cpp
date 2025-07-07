#ifdef COMPILE_TEST_ONLY
// Stub implementations for compilation testing
#include "MessageSender.h"
#include <iostream>

MessageSender::MessageSender() {
    std::cout << "MessageSender created (stub)" << std::endl;
}

MessageSender::~MessageSender() {
    std::cout << "MessageSender destroyed (stub)" << std::endl;
}

bool MessageSender::SendMessage(const std::string& message) { return true; }
bool MessageSender::SendMessageToProcess(const std::string& processName, const std::string& message) { return true; }
bool MessageSender::SetForegroundWindow(HWND hWnd) { return true; }
HWND MessageSender::FindWindowByProcess(const std::string& processName) { return nullptr; }
void MessageSender::SendEnterKey() { }
std::wstring MessageSender::StringToWString(const std::string& str) { return L""; }
void MessageSender::SendText(const std::wstring& text) { }
void MessageSender::SendKey(WORD vkCode) { }

#else
#include "MessageSender.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>

MessageSender::MessageSender() {
}

MessageSender::~MessageSender() {
}

bool MessageSender::SendMessage(const std::string& message) {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd) {
        return false;
    }
    
    // Set focus to the window
    SetForegroundWindow(hWnd);
    Sleep(50); // Small delay to ensure focus
    
    // Send the message
    std::wstring wideMessage = StringToWString(message);
    SendText(wideMessage);
    
    return true;
}

bool MessageSender::SendMessageToProcess(const std::string& processName, const std::string& message) {
    HWND hWnd = FindWindowByProcess(processName);
    if (!hWnd) {
        return false;
    }
    
    // Set focus to the window
    if (!SetForegroundWindow(hWnd)) {
        return false;
    }
    
    Sleep(50); // Small delay to ensure focus
    
    // Send the message
    std::wstring wideMessage = StringToWString(message);
    SendText(wideMessage);
    
    return true;
}

bool MessageSender::SetForegroundWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    
    return ::SetForegroundWindow(hWnd) != 0;
}

HWND MessageSender::FindWindowByProcess(const std::string& processName) {
    HWND hWnd = nullptr;
    
    // Create a snapshot of all processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return nullptr;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &pe32)) {
        do {
            // Convert process name to string for comparison
            std::string currentProcessName = pe32.szExeFile;
            
            // Case-insensitive comparison
            std::transform(currentProcessName.begin(), currentProcessName.end(), 
                         currentProcessName.begin(), ::tolower);
            std::string searchName = processName;
            std::transform(searchName.begin(), searchName.end(), 
                         searchName.begin(), ::tolower);
            
            if (currentProcessName.find(searchName) != std::string::npos) {
                // Found the process, now find its main window
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

void MessageSender::SendEnterKey() {
    SendKey(VK_RETURN);
}

std::wstring MessageSender::StringToWString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }
    
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    
    return wstr;
}

void MessageSender::SendText(const std::wstring& text) {
    std::vector<INPUT> inputs;
    inputs.reserve(text.length() * 2); // Each character requires key down and key up
    
    for (wchar_t ch : text) {
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = ch;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        inputs.push_back(input);
        
        // Key up event
        input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs.push_back(input);
    }
    
    if (!inputs.empty()) {
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
}

void MessageSender::SendKey(WORD vkCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = 0;
    
    // Key down
    SendInput(1, &input, sizeof(INPUT));
    
    // Key up
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

#endif // COMPILE_TEST_ONLY