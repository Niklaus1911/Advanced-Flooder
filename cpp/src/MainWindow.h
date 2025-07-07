#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef COMPILE_TEST_ONLY
// Stub declarations for compilation testing
typedef void* HINSTANCE;
typedef void* HWND;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long LPARAM;
typedef unsigned int WPARAM;
typedef long INT_PTR;
#define CALLBACK
#else
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

class MessageSender;
class ProcessEnumerator;

class MainWindow {
public:
    MainWindow(HINSTANCE hInstance);
    ~MainWindow();
    
    bool Initialize();
    void Show();
    void RunMessageLoop();
    
    // Event handlers
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    
private:
    HINSTANCE m_hInstance;
    HWND m_hDialog;
    
    // UI Controls
    HWND m_hProcessCombo;
    HWND m_hProcessReload;
    HWND m_hMessageText;
    HWND m_hMaxMessages;
    HWND m_hDelayInput;
    HWND m_hCurrentCount;
    HWND m_hStatusLabel;
    HWND m_hStartButton;
    HWND m_hStopButton;
    HWND m_hAboutButton;
    HWND m_hUseProcessCheck;
    
    // Core functionality
    std::unique_ptr<ICrossPlatformMessageSender> m_messageSender;
    std::unique_ptr<ICrossPlatformProcessEnumerator> m_processEnumerator;
    
    // Threading
    std::unique_ptr<std::thread> m_workerThread;
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_shouldStop;
    
    // State
    int m_currentCount;
    int m_maxMessages;
    int m_delay;
    std::string m_messageText;
    std::string m_targetProcess;
    bool m_useProcessTarget;
    
    // Dialog event handlers
    void OnInitDialog();
    void OnCommand(WPARAM wParam, LPARAM lParam);
    void OnClose();
    void OnDestroy();
    
    // Button handlers
    void OnStartClick();
    void OnStopClick();
    void OnAboutClick();
    void OnReloadProcesses();
    void OnUseProcessToggle();
    
    // UI helpers
    void UpdateUI();
    void UpdateStatus(const std::string& status, bool isActive);
    void UpdateMessageCount(int count);
    void PopulateProcessList();
    std::string GetWindowText(HWND hWnd);
    void SetWindowText(HWND hWnd, const std::string& text);
    
    // Worker thread
    void WorkerThread();
    
    // Input validation
    bool ValidateInput();
    
    // Static instance for dialog proc
    static MainWindow* s_instance;
};

#endif // MAINWINDOW_H