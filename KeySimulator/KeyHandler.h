#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#define WIN32_LEAN_AND_MEAN // Minimize Windows header size

#include <Windows.h>
#include <tchar.h>
#include <afxwin.h> // For MFC (CString, CWnd)
#include <string>
#include "StaticData.h"

struct KeyHandler {
    static KeyHandler* instance; // Static instance pointer

    bool g_listenForKey = false;
    HHOOK g_hHook = nullptr;
    UINT g_startKey = 0;
    int controlID;

    KeyHandler(int ctrlID);
    ~KeyHandler();

    static LRESULT CALLBACK KeyMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam); // Static callback
    CString GetKeyName(UINT vkCode);
    void SetHook();
    void UnsetHook();
    void SimulateKeyPress(CString* keys);
    void SimulateKey(const std::wstring& key);
};

#endif // !KEYHANDLER_H
