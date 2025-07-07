#ifdef __linux__
#include "CrossPlatform.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

class LinuxMessageSender : public ICrossPlatformMessageSender {
private:
    Display* display;
    
public:
    LinuxMessageSender() {
        display = XOpenDisplay(nullptr);
        if (!display) {
            std::cerr << "Failed to open X display" << std::endl;
        }
    }
    
    ~LinuxMessageSender() {
        if (display) {
            XCloseDisplay(display);
        }
    }
    
    bool SendMessageToActiveWindow(const std::string& message) override {
        if (!display) {
            std::cerr << "X display not available" << std::endl;
            return false;
        }
        
        // Get the currently focused window
        Window focused_window;
        int revert_to;
        XGetInputFocus(display, &focused_window, &revert_to);
        
        if (focused_window == None || focused_window == PointerRoot) {
            std::cerr << "No focused window found" << std::endl;
            return false;
        }
        
        return SendTextToWindow(focused_window, message);
    }
    
    bool SendMessageToProcess(const std::string& processName, const std::string& message) override {
        if (!display) {
            std::cerr << "X display not available" << std::endl;
            return false;
        }
        
        Window window = FindWindowByProcess(processName);
        if (window == None) {
            std::cerr << "Window for process '" << processName << "' not found" << std::endl;
            return false;
        }
        
        // Focus the window first
        XSetInputFocus(display, window, RevertToParent, CurrentTime);
        XFlush(display);
        Sleep(50); // Small delay to ensure focus
        
        return SendTextToWindow(window, message);
    }
    
    void SendEnterKey() override {
        if (!display) {
            return;
        }
        
        // Send Return key
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Return), True, CurrentTime);
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Return), False, CurrentTime);
        XFlush(display);
    }
    
    void Sleep(int milliseconds) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    
private:
    bool SendTextToWindow(Window window, const std::string& message) {
        if (!display) {
            return false;
        }
        
        // Focus the window
        XSetInputFocus(display, window, RevertToParent, CurrentTime);
        XFlush(display);
        Sleep(50);
        
        // Send each character of the message
        for (char c : message) {
            KeySym keysym = XStringToKeysym(std::string(1, c).c_str());
            if (keysym == NoSymbol) {
                // Handle special characters or use a fallback
                continue;
            }
            
            KeyCode keycode = XKeysymToKeycode(display, keysym);
            if (keycode == 0) {
                continue;
            }
            
            // Check if shift is needed for uppercase letters
            bool needShift = (c >= 'A' && c <= 'Z') || 
                           (c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || 
                            c == '^' || c == '&' || c == '*' || c == '(' || c == ')' ||
                            c == '_' || c == '+' || c == '{' || c == '}' || c == '|' ||
                            c == ':' || c == '"' || c == '<' || c == '>' || c == '?');
            
            if (needShift) {
                XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), True, CurrentTime);
            }
            
            XTestFakeKeyEvent(display, keycode, True, CurrentTime);
            XTestFakeKeyEvent(display, keycode, False, CurrentTime);
            
            if (needShift) {
                XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), False, CurrentTime);
            }
            
            XFlush(display);
            Sleep(1); // Small delay between characters
        }
        
        return true;
    }
    
    Window FindWindowByProcess(const std::string& processName) {
        // Get all windows
        std::vector<Window> windows = GetAllWindows();
        
        for (Window window : windows) {
            // Get the process ID of the window
            Atom atom = XInternAtom(display, "_NET_WM_PID", False);
            if (atom == None) continue;
            
            Atom actual_type;
            int actual_format;
            unsigned long nitems;
            unsigned long bytes_after;
            unsigned char* prop = nullptr;
            
            int status = XGetWindowProperty(display, window, atom, 0, 1, False, 
                                          XA_CARDINAL, &actual_type, &actual_format, 
                                          &nitems, &bytes_after, &prop);
            
            if (status == Success && prop) {
                pid_t pid = *((pid_t*)prop);
                XFree(prop);
                
                // Get the process name from /proc/[pid]/comm
                std::string procPath = "/proc/" + std::to_string(pid) + "/comm";
                std::ifstream file(procPath);
                if (file.is_open()) {
                    std::string currentProcessName;
                    std::getline(file, currentProcessName);
                    file.close();
                    
                    // Case-insensitive comparison
                    std::string lowerCurrent = currentProcessName;
                    std::string lowerSearch = processName;
                    std::transform(lowerCurrent.begin(), lowerCurrent.end(), 
                                 lowerCurrent.begin(), ::tolower);
                    std::transform(lowerSearch.begin(), lowerSearch.end(), 
                                 lowerSearch.begin(), ::tolower);
                    
                    if (lowerCurrent.find(lowerSearch) != std::string::npos) {
                        return window;
                    }
                }
            }
        }
        
        return None;
    }
    
    std::vector<Window> GetAllWindows() {
        std::vector<Window> windows;
        
        Window root = DefaultRootWindow(display);
        Window parent;
        Window* children;
        unsigned int nchildren;
        
        if (XQueryTree(display, root, &root, &parent, &children, &nchildren) == 0) {
            return windows;
        }
        
        for (unsigned int i = 0; i < nchildren; i++) {
            // Check if window is visible and has a title
            XWindowAttributes attrs;
            if (XGetWindowAttributes(display, children[i], &attrs) == 0) {
                continue;
            }
            
            if (attrs.map_state == IsViewable) {
                char* window_name = nullptr;
                if (XFetchName(display, children[i], &window_name) && window_name) {
                    windows.push_back(children[i]);
                    XFree(window_name);
                }
            }
        }
        
        if (children) {
            XFree(children);
        }
        
        return windows;
    }
};

class LinuxProcessEnumerator : public ICrossPlatformProcessEnumerator {
public:
    std::vector<ProcessInfo> GetProcesses() override {
        std::vector<ProcessInfo> processes;
        
        DIR* proc_dir = opendir("/proc");
        if (!proc_dir) {
            return processes;
        }
        
        struct dirent* entry;
        while ((entry = readdir(proc_dir)) != nullptr) {
            // Check if directory name is a number (PID)
            if (strspn(entry->d_name, "0123456789") != strlen(entry->d_name)) {
                continue;
            }
            
            pid_t pid = std::stoi(entry->d_name);
            ProcessInfo info;
            info.processId = pid;
            info.hasWindow = false;
            
            // Read process name from /proc/[pid]/comm
            std::string commPath = "/proc/" + std::string(entry->d_name) + "/comm";
            std::ifstream commFile(commPath);
            if (commFile.is_open()) {
                std::getline(commFile, info.name);
                commFile.close();
                
                // Check if process has a window
                info.hasWindow = ProcessHasWindow(pid);
                if (info.hasWindow) {
                    info.windowTitle = GetWindowTitle(pid);
                }
                
                processes.push_back(info);
            }
        }
        
        closedir(proc_dir);
        
        // Sort by name
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
    
private:
    bool ProcessHasWindow(pid_t pid) {
        // This is a simplified check - in a real implementation, you'd need to
        // check X11 windows to see if any belong to this process
        Display* display = XOpenDisplay(nullptr);
        if (!display) {
            return false;
        }
        
        bool hasWindow = false;
        
        // Get all windows and check if any belong to this process
        Window root = DefaultRootWindow(display);
        Window parent;
        Window* children;
        unsigned int nchildren;
        
        if (XQueryTree(display, root, &root, &parent, &children, &nchildren) != 0) {
            for (unsigned int i = 0; i < nchildren; i++) {
                Atom atom = XInternAtom(display, "_NET_WM_PID", False);
                if (atom == None) continue;
                
                Atom actual_type;
                int actual_format;
                unsigned long nitems;
                unsigned long bytes_after;
                unsigned char* prop = nullptr;
                
                int status = XGetWindowProperty(display, children[i], atom, 0, 1, False, 
                                              XA_CARDINAL, &actual_type, &actual_format, 
                                              &nitems, &bytes_after, &prop);
                
                if (status == Success && prop) {
                    pid_t window_pid = *((pid_t*)prop);
                    XFree(prop);
                    
                    if (window_pid == pid) {
                        hasWindow = true;
                        break;
                    }
                }
            }
            
            if (children) {
                XFree(children);
            }
        }
        
        XCloseDisplay(display);
        return hasWindow;
    }
    
    std::string GetWindowTitle(pid_t pid) {
        Display* display = XOpenDisplay(nullptr);
        if (!display) {
            return "";
        }
        
        std::string title;
        
        // Get all windows and find the one belonging to this process
        Window root = DefaultRootWindow(display);
        Window parent;
        Window* children;
        unsigned int nchildren;
        
        if (XQueryTree(display, root, &root, &parent, &children, &nchildren) != 0) {
            for (unsigned int i = 0; i < nchildren; i++) {
                Atom atom = XInternAtom(display, "_NET_WM_PID", False);
                if (atom == None) continue;
                
                Atom actual_type;
                int actual_format;
                unsigned long nitems;
                unsigned long bytes_after;
                unsigned char* prop = nullptr;
                
                int status = XGetWindowProperty(display, children[i], atom, 0, 1, False, 
                                              XA_CARDINAL, &actual_type, &actual_format, 
                                              &nitems, &bytes_after, &prop);
                
                if (status == Success && prop) {
                    pid_t window_pid = *((pid_t*)prop);
                    XFree(prop);
                    
                    if (window_pid == pid) {
                        char* window_name = nullptr;
                        if (XFetchName(display, children[i], &window_name) && window_name) {
                            title = window_name;
                            XFree(window_name);
                            break;
                        }
                    }
                }
            }
            
            if (children) {
                XFree(children);
            }
        }
        
        XCloseDisplay(display);
        return title;
    }
};

ICrossPlatformMessageSender* CreateMessageSender() {
    return new LinuxMessageSender();
}

ICrossPlatformProcessEnumerator* CreateProcessEnumerator() {
    return new LinuxProcessEnumerator();
}

#endif // __linux__