#ifdef COMPILE_TEST_ONLY
// Stub implementations for compilation testing on non-Windows platforms
#include <iostream>
#include <string>

// Dummy Windows types and functions
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HANDLE;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned short WORD;
typedef long LPARAM;
typedef unsigned int WPARAM;
typedef long LRESULT;
typedef long INT_PTR;
typedef long LONG_PTR;
typedef struct { int x, y; } POINT;
typedef struct { LONG left, top, right, bottom; } RECT;
typedef struct { UINT type; struct { WORD wVk; WORD wScan; DWORD dwFlags; } ki; } INPUT;
typedef struct { DWORD dwSize; char szExeFile[260]; DWORD th32ProcessID; } PROCESSENTRY32;
typedef struct { DWORD dwSize; DWORD dwICC; } INITCOMMONCONTROLSEX;
typedef struct { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; } MSG;

#define WINAPI
#define CALLBACK
#define SW_SHOW 1
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_CLOSE 0x0010
#define WM_DESTROY 0x0002
#define WM_KEYDOWN 0x0100
#define WM_USER 0x0400
#define MB_OK 0x00000000L
#define MB_ICONERROR 0x00000010L
#define MB_ICONINFORMATION 0x00000040L
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_RETURN 0x0D
#define TRUE 1
#define FALSE 0
#define HWND_TOPMOST ((HWND)-1)
#define SWP_NOMOVE 0x0002
#define SWP_NOSIZE 0x0001
#define BST_CHECKED 0x0001
#define CB_ERR (-1)
#define CB_RESETCONTENT 0x014B
#define CB_ADDSTRING 0x0143
#define CB_SETCURSEL 0x014E
#define CB_GETCURSEL 0x0147
#define CB_GETLBTEXT 0x0148
#define MAKEINTRESOURCE(i) ((char*)(i))
#define IDD_MAIN_DIALOG 100
#define MAX_PATH 260
#define CP_UTF8 65001
#define INPUT_KEYBOARD 1
#define KEYEVENTF_UNICODE 0x0004
#define KEYEVENTF_KEYUP 0x0002
#define PROCESS_QUERY_INFORMATION 0x0400
#define PROCESS_VM_READ 0x0010
#define TH32CS_SNAPPROCESS 0x00000002
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#define LOWORD(l) ((WORD)(((DWORD)(l)) & 0xffff))
#define ICC_STANDARD_CLASSES 0x00004000

// Dummy function declarations
HWND CreateDialog(HINSTANCE, const char*, HWND, INT_PTR (CALLBACK*)(HWND, UINT, WPARAM, LPARAM)) { return nullptr; }
HWND GetDlgItem(HWND, int) { return nullptr; }
int MessageBox(HWND, const wchar_t*, const wchar_t*, UINT) { return 0; }
BOOL ShowWindow(HWND, int) { return TRUE; }
BOOL UpdateWindow(HWND) { return TRUE; }
BOOL GetMessage(MSG*, HWND, UINT, UINT) { return FALSE; }
BOOL IsDialogMessage(HWND, MSG*) { return FALSE; }
BOOL TranslateMessage(MSG*) { return TRUE; }
LRESULT DispatchMessage(MSG*) { return 0; }
BOOL SetWindowPos(HWND, HWND, int, int, int, int, UINT) { return TRUE; }
BOOL EnableWindow(HWND, BOOL) { return TRUE; }
BOOL IsDlgButtonChecked(HWND, int) { return FALSE; }
int GetWindowTextLength(HWND) { return 0; }
int GetWindowTextA(HWND, char*, int) { return 0; }
BOOL SetWindowTextA(HWND, const char*) { return TRUE; }
LRESULT SendMessage(HWND, UINT, WPARAM, LPARAM) { return 0; }
BOOL PostMessage(HWND, UINT, WPARAM, LPARAM) { return TRUE; }
BOOL DestroyWindow(HWND) { return TRUE; }
void PostQuitMessage(int) { }
HWND GetForegroundWindow() { return nullptr; }
BOOL SetForegroundWindow(HWND) { return TRUE; }
BOOL IsWindow(HWND) { return FALSE; }
BOOL IsWindowVisible(HWND) { return FALSE; }
UINT SendInput(UINT, INPUT*, int) { return 0; }
HANDLE CreateToolhelp32Snapshot(DWORD, DWORD) { return INVALID_HANDLE_VALUE; }
BOOL Process32First(HANDLE, PROCESSENTRY32*) { return FALSE; }
BOOL Process32Next(HANDLE, PROCESSENTRY32*) { return FALSE; }
BOOL CloseHandle(HANDLE) { return TRUE; }
BOOL EnumWindows(BOOL (CALLBACK*)(HWND, LPARAM), LPARAM) { return TRUE; }
DWORD GetWindowThreadProcessId(HWND, DWORD*) { return 0; }
HANDLE OpenProcess(DWORD, BOOL, DWORD) { return nullptr; }
DWORD GetModuleBaseNameA(HANDLE, void*, char*, DWORD) { return 0; }
BOOL InitCommonControlsEx(INITCOMMONCONTROLSEX*) { return TRUE; }
int MultiByteToWideChar(UINT, DWORD, const char*, int, wchar_t*, int) { return 0; }
int WideCharToMultiByte(UINT, DWORD, const wchar_t*, int, char*, int, const char*, BOOL*) { return 0; }
void Sleep(DWORD) { }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow) {
    std::cout << "Advanced Flooder - C++ Version (Compilation Test)" << std::endl;
    std::cout << "This is a Windows-only application. Compiled successfully for testing." << std::endl;
    return 0;
}

// For non-Windows compilation testing
int main() {
    std::cout << "Advanced Flooder - C++ Version (Compilation Test)" << std::endl;
    std::cout << "This is a Windows-only application. Compiled successfully for testing." << std::endl;
    return 0;
}

#else

#include <windows.h>
#include <commctrl.h>
#include "MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    // Create main window
    MainWindow window(hInstance);
    
    if (!window.Initialize()) {
        MessageBox(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Show window and run message loop
    window.Show();
    window.RunMessageLoop();
    
    return 0;
}

#endif // COMPILE_TEST_ONLY