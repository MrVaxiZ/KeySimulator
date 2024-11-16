#include "KeyHandler.h"

KeyHandler* KeyHandler::instance = nullptr; // Initialize the static instance

KeyHandler::KeyHandler(int ctrlID) : controlID(ctrlID) {
    instance = this; // Set the static instance pointer to the current instance
}

KeyHandler::~KeyHandler() {
    UnsetHook();
}

void KeyHandler::SetHook() {
    g_listenForKey = true;
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyMouseHookProc, nullptr, 0);
}

void KeyHandler::UnsetHook() {
    if (g_hHook != nullptr) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }
}

// Static callback function that accesses instance members via the instance pointer
LRESULT CALLBACK KeyHandler::KeyMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && instance->g_listenForKey) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            instance->g_startKey = kbStruct->vkCode;

            // Update button text
            CWnd* pButton = AfxGetMainWnd()->GetDlgItem(instance->controlID);
            if (pButton != nullptr) {
                CString keyName = instance->GetKeyName(instance->g_startKey);
                pButton->SetWindowText(_T("Start Key: ") + keyName);
            }

            instance->g_listenForKey = false;
            instance->UnsetHook();
            AfxMessageBox(_T("Start key set!"));
        }
        else if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) {
            instance->g_startKey = wParam;

            // Update button text with mouse button information
            CWnd* pButton = AfxGetMainWnd()->GetDlgItem(instance->controlID);
            if (pButton != nullptr) {
                CString buttonText = (wParam == WM_LBUTTONDOWN) ? _T("Start Key: Left Mouse") : _T("Start Key: Right Mouse");
                pButton->SetWindowText(buttonText);
            }

            instance->g_listenForKey = false;
            instance->UnsetHook();
            AfxMessageBox(_T("Start button set!"));
        }
    }
    return CallNextHookEx(instance->g_hHook, nCode, wParam, lParam);
}

CString KeyHandler::GetKeyName(UINT vkCode) {
    TCHAR keyName[32] = { 0 };
    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

    if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName) / sizeof(TCHAR))) {
        StaticData::activationKey = CString(keyName);
        return CString(keyName);
    }
    return _T("Unknown Key");
}

void KeyHandler::SimulateKeyPress(CString* keys) {
    CString data = *keys;
    std::wstring input = data.GetString(); // Convert CString to std::wstring

    size_t pos = 0; // Position tracker
    std::wstring token;
    while ((pos = input.find(L',')) != std::wstring::npos) {
        token = input.substr(0, pos); // Extract substring before the comma
        if (!token.empty()) {
            // Simulate the extracted token
            SimulateKey(token);
        }
        input.erase(0, pos + 1); // Erase processed part, including the comma
    }

    // Simulate any remaining part of the string after the last comma
    if (!input.empty()) {
        SimulateKey(input);
    }
}

// Simulates a string of keys, assuming 'masl' or 'o' are valid keys
void KeyHandler::SimulateKey(const std::wstring& key) {
    for (wchar_t ch : key) {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;            // Use Virtual-Key Code if needed
        input.ki.wScan = ch;         // Use the character as a scan code
        input.ki.dwFlags = KEYEVENTF_UNICODE; // Specify Unicode character

        // Simulate key press
        SendInput(1, &input, sizeof(INPUT));

        // Simulate key release
        input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}