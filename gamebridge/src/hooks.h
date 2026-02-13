#pragma once
#include <windows.h>

#include "game_bridge.h"

class GAME_BRIDGE_API WindowHooks {

    inline static bool is_hooked = false;

public:
    WindowHooks();

    void ActivateWindowMessageHook(HWND h_wnd = nullptr);
    void RestoreWindowMessageHook();

    void OpenConsole();

    void CloseConsole();
};
