#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

// Function to find the game process ID by executable name
DWORD GetProcessIdByName(const std::wstring& processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return processId;
}

int main() {
    std::wcout << L"=== Dawnwalker Advanced Trainer v1.0.0 ===" << std::endl;
    std::wcout << L"Searching for game process..." << std::endl;

    // Target game process name
    std::wstring gameProcessName = L"Dawnwalker-Win64-Shipping.exe"; 
    DWORD processId = GetProcessIdByName(gameProcessName);

    if (processId == 0) {
        std::wcout << L"[-] Game process not found! Please launch the game first." << std::endl;
        std::system("pause");
        return 1;
    }

    std::wcout << L"[+] Game found! Process ID: " << processId << std::endl;

    // Open process with read/write memory privileges
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::wcout << L"[-] Failed to open process. Run this trainer as Administrator!" << std::endl;
        std::system("pause");
        return 1;
    }

    std::wcout << L"[+] Trainer successfully attached. Press Hotkeys to toggle features." << std::endl;
    std::wcout << L"F1: God Mode | F4: One-Hit Kill | END: Exit" << std::endl;

    bool godModeActive = false;

    // Main hotkey tracking loop
    while (true) {
        // Check for F1 key press (God Mode)
        if (GetAsyncKeyState(VK_F1) & 1) {
            godModeActive = !godModeActive;
            
            // Example memory address and offsets (mock values for template structure)
            uintptr_t baseAddress = 0x7FFA12345678; 
            int newValue = godModeActive ? 99999 : 100;

            // Write new value to the game process memory
            BOOL success = WriteProcessMemory(hProcess, (LPVOID)baseAddress, &newValue, sizeof(newValue), NULL);

            if (success) {
                std::wcout << L"[Status] God Mode: " << (godModeActive ? L"ENABLED" : L"DISABLED") << std::endl;
            } else {
                std::wcout << L"[-] Failed to write memory." << std::endl;
            }
        }

        // Exit trainer loop when END key is pressed
        if (GetAsyncKeyState(VK_END)) {
            break;
        }

        Sleep(100); // Reduce CPU usage
    }

    CloseHandle(hProcess);
    return 0;
}
