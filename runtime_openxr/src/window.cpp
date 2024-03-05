#include "window.h"

namespace XRGameBridge {
    void MessageLoop() {
        // Main message loop:
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT CALLBACK GB_Display::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        PAINTSTRUCT ps;
        HDC hdc;
        std::string greeting("Hello, Windows desktop!");

        switch (message) {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);

            // Here your application is laid out.
            // For this introduction, we just print out "Hello, Windows desktop!"
            // in the top left corner.
            TextOut(hdc, 5, 5, greeting.data(), (greeting.size()));
            // End application-specific layout section.

            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_QUIT:
            ShowWindow(hWnd, false);
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

        return 0;
    }

    bool GB_Display::CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow) {
        // TODO better window creation checking code
        static bool window_created = false;
        if (window_created) {
            return false;
        }

        window_created = true;

        // Create window
        uint32_t window_style = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

        WNDCLASSEX window_ex;

        window_ex.cbSize = sizeof(WNDCLASSEX);
        window_ex.style = CS_HREDRAW | CS_VREDRAW;// | window_style;
        window_ex.lpfnWndProc = WndProc;
        window_ex.cbClsExtra = 0;
        window_ex.cbWndExtra = 0;
        window_ex.hInstance = hInstance;
        window_ex.hIcon = LoadIcon(window_ex.hInstance, IDI_APPLICATION);
        window_ex.hCursor = LoadCursor(NULL, IDC_ARROW);
        window_ex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        window_ex.lpszMenuName = NULL;
        window_ex.lpszClassName = window_class.c_str();
        window_ex.hIconSm = LoadIcon(window_ex.hInstance, IDI_APPLICATION);

        if (!RegisterClassEx(&window_ex)) {
            MessageBox(NULL, "Call to RegisterClassEx failed!", "XR Game Bridge", NULL);

            return false;
        }

        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);
        h_wnd = CreateWindowEx(0, window_class.c_str(), title.c_str(), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, hInstance, NULL);
        if (!h_wnd) {
            MessageBox(NULL, "Call to CreateWindow failed!", "XR Game Bridge", NULL);
            return false;
        }

        SetWindowLongPtr(h_wnd, GWL_STYLE, window_style); //3d argument=style

        // The parameters to ShowWindow explained:
        // h_wnd: the value returned from CreateWindow
        // nCmdShow: the fourth parameter from WinMain
        ShowWindow(h_wnd, nCmdShow);

        return true;
    }

    HWND GB_Display::GetWindowHandle() {
        return h_wnd;
    }

    void GB_Display::UpdateWindow() {
        // Main message loop:
        MSG msg;
        if (PeekMessageA(&msg, h_wnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
