#pragma once
#include "openxr_includes.h"

namespace XRGameBridge {
    class GB_Display {
        // The main window class name.
        std::string window_class = "Game Bridge Window";

        // The string that appears in the application's title bar.
        std::string title = "XR Game Bridge";

        HWND h_wnd = 0;

    public:
        // Returns the window of the application or false if none exist
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        bool CreateApplicationWindow(HINSTANCE hInstance, uint32_t width, uint32_t height, int nCmdShow, bool fullscreen = true);
        HWND GetWindowHandle();
        void UpdateWindow();
    };
}
