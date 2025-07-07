#ifdef COMPILE_TEST_ONLY
// Stub implementations for compilation testing
#include "MainWindow.h"
#include "MessageSender.h"
#include "ProcessEnumerator.h"
#include <iostream>

#define FALSE 0
#define TRUE 1

MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow(HINSTANCE hInstance) 
    : m_hInstance(hInstance), m_hDialog(nullptr), m_isRunning(false), m_shouldStop(false),
      m_currentCount(0), m_maxMessages(10), m_delay(100), m_useProcessTarget(false) {
    std::cout << "MainWindow created (stub)" << std::endl;
}

MainWindow::~MainWindow() {
    std::cout << "MainWindow destroyed (stub)" << std::endl;
}

bool MainWindow::Initialize() { return true; }
void MainWindow::Show() { }
void MainWindow::RunMessageLoop() { }
INT_PTR CALLBACK MainWindow::DialogProc(HWND, UINT, WPARAM, LPARAM) { return FALSE; }
void MainWindow::OnInitDialog() { }
void MainWindow::OnCommand(WPARAM, LPARAM) { }
void MainWindow::OnClose() { }
void MainWindow::OnDestroy() { }
void MainWindow::OnStartClick() { }
void MainWindow::OnStopClick() { }
void MainWindow::OnAboutClick() { }
void MainWindow::OnReloadProcesses() { }
void MainWindow::OnUseProcessToggle() { }
void MainWindow::UpdateUI() { }
void MainWindow::UpdateStatus(const std::string&, bool) { }
void MainWindow::UpdateMessageCount(int) { }
void MainWindow::PopulateProcessList() { }
std::string MainWindow::GetWindowText(HWND) { return ""; }
void MainWindow::SetWindowText(HWND, const std::string&) { }
void MainWindow::WorkerThread() { }
bool MainWindow::ValidateInput() { return true; }

#else
#include "MainWindow.h"
#include "CrossPlatform.h"
#include "../resources/resource.h"
#include <sstream>

// Static member initialization
MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow(HINSTANCE hInstance) 
    : m_hInstance(hInstance), m_hDialog(nullptr), m_isRunning(false), m_shouldStop(false),
      m_currentCount(0), m_maxMessages(10), m_delay(100), m_useProcessTarget(false) {
    s_instance = this;
    m_messageSender.reset(CreateMessageSender());
    m_processEnumerator.reset(CreateProcessEnumerator());
    m_messageText = "Default message";
}

MainWindow::~MainWindow() {
    if (m_workerThread && m_workerThread->joinable()) {
        m_shouldStop = true;
        m_workerThread->join();
    }
    s_instance = nullptr;
}

bool MainWindow::Initialize() {
    m_hDialog = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), nullptr, DialogProc);
    if (!m_hDialog) {
        return false;
    }
    
    // Get control handles
    m_hProcessCombo = GetDlgItem(m_hDialog, IDC_PROCESS_COMBO);
    m_hProcessReload = GetDlgItem(m_hDialog, IDC_PROCESS_RELOAD);
    m_hMessageText = GetDlgItem(m_hDialog, IDC_MESSAGE_TEXT);
    m_hMaxMessages = GetDlgItem(m_hDialog, IDC_MAX_MESSAGES);
    m_hDelayInput = GetDlgItem(m_hDialog, IDC_DELAY_INPUT);
    m_hCurrentCount = GetDlgItem(m_hDialog, IDC_CURRENT_COUNT);
    m_hStatusLabel = GetDlgItem(m_hDialog, IDC_STATUS_LABEL);
    m_hStartButton = GetDlgItem(m_hDialog, IDC_START_BUTTON);
    m_hStopButton = GetDlgItem(m_hDialog, IDC_STOP_BUTTON);
    m_hAboutButton = GetDlgItem(m_hDialog, IDC_ABOUT_BUTTON);
    m_hUseProcessCheck = GetDlgItem(m_hDialog, IDC_USE_PROCESS_CHECK);
    
    return true;
}

void MainWindow::Show() {
    if (m_hDialog) {
        ShowWindow(m_hDialog, SW_SHOW);
        UpdateWindow(m_hDialog);
    }
}

void MainWindow::RunMessageLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!IsDialogMessage(m_hDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

INT_PTR CALLBACK MainWindow::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    if (s_instance) {
        switch (message) {
            case WM_INITDIALOG:
                s_instance->OnInitDialog();
                return TRUE;
                
            case WM_COMMAND:
                s_instance->OnCommand(wParam, lParam);
                return TRUE;
                
            case WM_CLOSE:
                s_instance->OnClose();
                return TRUE;
                
            case WM_DESTROY:
                s_instance->OnDestroy();
                return TRUE;
                
            case WM_KEYDOWN:
                if (wParam == VK_F5) {
                    s_instance->OnStartClick();
                    return TRUE;
                } else if (wParam == VK_F6) {
                    s_instance->OnStopClick();
                    return TRUE;
                }
                break;
        }
    }
    return FALSE;
}

void MainWindow::OnInitDialog() {
    // Set default values
    SetWindowText(m_hMaxMessages, "10");
    SetWindowText(m_hDelayInput, "100");
    SetWindowText(m_hMessageText, "Default message");
    
    // Initialize status
    UpdateStatus("Inactive", false);
    
    // Populate process list
    PopulateProcessList();
    
    // Set initial button states
    EnableWindow(m_hStopButton, FALSE);
    EnableWindow(m_hProcessCombo, FALSE);
    EnableWindow(m_hProcessReload, FALSE);
    
    // Set window to always on top
    SetWindowPos(m_hDialog, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(wParam)) {
        case IDC_START_BUTTON:
            OnStartClick();
            break;
            
        case IDC_STOP_BUTTON:
            OnStopClick();
            break;
            
        case IDC_ABOUT_BUTTON:
            OnAboutClick();
            break;
            
        case IDC_PROCESS_RELOAD:
            OnReloadProcesses();
            break;
            
        case IDC_USE_PROCESS_CHECK:
            OnUseProcessToggle();
            break;
    }
}

void MainWindow::OnClose() {
    m_shouldStop = true;
    if (m_workerThread && m_workerThread->joinable()) {
        m_workerThread->join();
    }
    DestroyWindow(m_hDialog);
}

void MainWindow::OnDestroy() {
    PostQuitMessage(0);
}

void MainWindow::OnStartClick() {
    if (!ValidateInput()) {
        return;
    }
    
    m_shouldStop = false;
    m_isRunning = true;
    m_currentCount = 0;
    
    // Update UI
    UpdateStatus("Active", true);
    UpdateMessageCount(0);
    EnableWindow(m_hStartButton, FALSE);
    EnableWindow(m_hStopButton, TRUE);
    
    // Start worker thread
    m_workerThread = std::make_unique<std::thread>(&MainWindow::WorkerThread, this);
}

void MainWindow::OnStopClick() {
    m_shouldStop = true;
    m_isRunning = false;
    
    // Update UI
    UpdateStatus("Inactive", false);
    EnableWindow(m_hStartButton, TRUE);
    EnableWindow(m_hStopButton, FALSE);
    
    // Wait for worker thread to finish
    if (m_workerThread && m_workerThread->joinable()) {
        m_workerThread->join();
    }
}

void MainWindow::OnAboutClick() {
    MessageBox(m_hDialog, 
               L"Advanced Flooder\r\n"
               L"Author: Niklaus1911\r\n"
               L"All rights reserved.\r\n"
               L"Website: www.igdownloader.com\r\n\r\n"
               L"C++ Port",
               L"Advanced Flooder", 
               MB_OK | MB_ICONINFORMATION);
}

void MainWindow::OnReloadProcesses() {
    PopulateProcessList();
}

void MainWindow::OnUseProcessToggle() {
    BOOL checked = IsDlgButtonChecked(m_hDialog, IDC_USE_PROCESS_CHECK);
    m_useProcessTarget = (checked == BST_CHECKED);
    
    EnableWindow(m_hProcessCombo, m_useProcessTarget);
    EnableWindow(m_hProcessReload, m_useProcessTarget);
}

void MainWindow::UpdateUI() {
    // This method can be called from worker thread, so we need to be careful
    // For now, we'll update the count directly in the worker thread
}

void MainWindow::UpdateStatus(const std::string& status, bool isActive) {
    SetWindowText(m_hStatusLabel, status.c_str());
    
    std::string title = "Advanced Flooder - " + status;
    SetWindowTextA(m_hDialog, title.c_str());
    
    // Update label color would require custom drawing or using a different approach
    // For now, we'll just update the text
}

void MainWindow::UpdateMessageCount(int count) {
    SetWindowText(m_hCurrentCount, std::to_string(count).c_str());
}

void MainWindow::PopulateProcessList() {
    // Clear existing items
    SendMessage(m_hProcessCombo, CB_RESETCONTENT, 0, 0);
    
    // Get processes with windows
    std::vector<ProcessInfo> processes = m_processEnumerator->GetProcessesWithWindows();
    
    // Add processes to combo box
    for (const auto& process : processes) {
        SendMessage(m_hProcessCombo, CB_ADDSTRING, 0, (LPARAM)process.name.c_str());
    }
    
    // Select first item if available
    if (!processes.empty()) {
        SendMessage(m_hProcessCombo, CB_SETCURSEL, 0, 0);
    }
}

std::string MainWindow::GetWindowText(HWND hWnd) {
    int length = GetWindowTextLength(hWnd);
    if (length == 0) {
        return "";
    }
    
    std::vector<char> buffer(length + 1);
    GetWindowTextA(hWnd, buffer.data(), length + 1);
    return std::string(buffer.data());
}

void MainWindow::SetWindowText(HWND hWnd, const std::string& text) {
    SetWindowTextA(hWnd, text.c_str());
}

void MainWindow::WorkerThread() {
    while (!m_shouldStop && m_isRunning) {
        // Get current values from UI
        m_messageText = GetWindowText(m_hMessageText);
        m_maxMessages = std::stoi(GetWindowText(m_hMaxMessages));
        m_delay = std::stoi(GetWindowText(m_hDelayInput));
        
        if (m_useProcessTarget) {
            // Get selected process name
            int selectedIndex = SendMessage(m_hProcessCombo, CB_GETCURSEL, 0, 0);
            if (selectedIndex != CB_ERR) {
                std::vector<char> buffer(256);
                SendMessage(m_hProcessCombo, CB_GETLBTEXT, selectedIndex, (LPARAM)buffer.data());
                m_targetProcess = std::string(buffer.data());
                
                // Send message to specific process
                m_messageSender->SendMessageToProcess(m_targetProcess, m_messageText);
            }
        } else {
            // Send message to foreground window
            m_messageSender->SendMessageToActiveWindow(m_messageText);
        }
        
        // Send Enter key
        m_messageSender->SendEnterKey();
        
        // Update count
        m_currentCount++;
        
        // Update UI (must be done on main thread)
        PostMessage(m_hDialog, WM_USER + 1, m_currentCount, 0);
        
        // Check if we've reached the maximum
        if (m_currentCount >= m_maxMessages) {
            m_isRunning = false;
            break;
        }
        
        // Sleep for the specified delay
        m_messageSender->Sleep(m_delay);
    }
    
    // Update UI when done
    PostMessage(m_hDialog, WM_USER + 2, 0, 0);
}

bool MainWindow::ValidateInput() {
    // Get delay value
    std::string delayText = GetWindowText(m_hDelayInput);
    try {
        int delay = std::stoi(delayText);
        if (delay < 50) {
            MessageBox(m_hDialog, L"Minimum delay is 50 ms", L"Advanced Flooder", MB_OK | MB_ICONERROR);
            return false;
        }
    } catch (...) {
        MessageBox(m_hDialog, L"Invalid delay value", L"Advanced Flooder", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // Get max messages value
    std::string maxText = GetWindowText(m_hMaxMessages);
    try {
        int maxMessages = std::stoi(maxText);
        if (maxMessages <= 0) {
            MessageBox(m_hDialog, L"Max messages must be greater than 0", L"Advanced Flooder", MB_OK | MB_ICONERROR);
            return false;
        }
    } catch (...) {
        MessageBox(m_hDialog, L"Invalid max messages value", L"Advanced Flooder", MB_OK | MB_ICONERROR);
        return false;
    }
    
    // Check if message text is not empty
    std::string messageText = GetWindowText(m_hMessageText);
    if (messageText.empty()) {
        MessageBox(m_hDialog, L"Message text cannot be empty", L"Advanced Flooder", MB_OK | MB_ICONERROR);
        return false;
    }
    
    return true;
}

#endif // COMPILE_TEST_ONLY