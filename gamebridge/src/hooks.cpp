#include "hooks.h"

#include <iostream>
#include <safetyhook.hpp>

inline SafetyHookInline safe_hook_PeekMessageA {};
inline SafetyHookInline safe_hook_PeekMessageW {};

BOOL HookFunc_PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    std::cout << "Hooked A" << std::endl;
    return safe_hook_PeekMessageA.call<BOOL>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

BOOL HookFunc_PeekMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    std::cout << "Hooked W" << std::endl;
    return safe_hook_PeekMessageW.call<BOOL>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

WindowHooks::WindowHooks() {
}

void WindowHooks::ActivateWindowMessageHook(HWND h_wnd) {
    if (is_hooked) {
        return;
    }
    safe_hook_PeekMessageA = safetyhook::create_inline(reinterpret_cast<void*>(PeekMessageA), reinterpret_cast<void*>(HookFunc_PeekMessageA));
    safe_hook_PeekMessageW = safetyhook::create_inline(reinterpret_cast<void*>(PeekMessageW), reinterpret_cast<void*>(HookFunc_PeekMessageW));

    is_hooked = true;
}

void WindowHooks::RestoreWindowMessageHook() {
    if (is_hooked) {
        safe_hook_PeekMessageA = {};
        safe_hook_PeekMessageW = {};

        is_hooked = false;
    }
}

void WindowHooks::OpenConsole() {
    AllocConsole();
    //SetConsoleTitleA("XRGameBridge Console");
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    std::cout << "Console initialized!";
}

void WindowHooks::CloseConsole() {
    FreeConsole();
}
