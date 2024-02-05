#include "swapchain.h"

#include <stdexcept>
#include <vector>

#include "easylogging++.h"
#include "openxr_functions.h"
#include "instance.h"
#include "system.h"

XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats) {
    GameBridge::GraphicsBackend backend;

    try {
        GameBridge::GB_Session& gb_session = GameBridge::sessions.at(session);
        GameBridge::GB_System& gb_system = GameBridge::systems.at(gb_session.system);
        backend = gb_system.active_graphics_backend;
    }
    catch (std::out_of_range& e) {
        return XR_ERROR_SYSTEM_INVALID;
    }
    catch (std::exception& e) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    std::vector<int64_t> supported_swapchain_formats;
    if (backend == GameBridge::GraphicsBackend::D3D12) {
        supported_swapchain_formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
        supported_swapchain_formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    }
    if (backend == GameBridge::GraphicsBackend::D3D11) {
        // not implemented
        return XR_ERROR_RUNTIME_FAILURE;
    }

    *formatCountOutput = supported_swapchain_formats.size();

    if (formatCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    if (formatCapacityInput < 1) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    // Fill array
    memcpy_s(formats, formatCapacityInput * sizeof(int64_t), supported_swapchain_formats.data(), supported_swapchain_formats.size() * sizeof(int64_t));
    return XR_SUCCESS;
}

XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain) {


    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrDestroySwapchain(XrSwapchain swapchain) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, XrPath* sources) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo* waitInfo) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo) {
    return XR_ERROR_RUNTIME_FAILURE;
}

namespace GameBridge {
    std::vector<IDXGIAdapter*> EnumerateAdapters() {
        IDXGIAdapter* adapter;
        std::vector<IDXGIAdapter*> adapters;
        IDXGIFactory1* factory = NULL;

        // Create a DXGIFactory object.
        HRESULT err = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
        if (FAILED(err)) {
            LOG(ERROR) << "Could not create DXGIFactory with error: " << err;
            return adapters;
        }

        for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            adapters.push_back(adapter);
        }

        if (factory) {
            factory->Release();
        }

        return adapters;
    }

    std::map<uint32_t, DXGI_ADAPTER_DESC> DetermineDeviceScores(std::vector<IDXGIAdapter*> adapters) {
        // VRAM
        uint32_t dedicated_memory_modifier = 2;
        // RAM used only by the GPU (integrated graphics)
        uint32_t dedicated_system_memory_modifier = 1;
        // CPU ram shared with the GPU
        uint32_t shared_memory_modifier = 0.1;

        std::map<uint32_t, DXGI_ADAPTER_DESC> adapter_scores;
        for (auto adapter : adapters) {
            DXGI_ADAPTER_DESC adapter_desc;
            adapter->GetDesc(&adapter_desc);

            uint32_t adapter_score = 0;

            adapter_score += dedicated_memory_modifier * adapter_desc.DedicatedVideoMemory;
            adapter_score += dedicated_system_memory_modifier * adapter_desc.DedicatedSystemMemory;
            adapter_score += shared_memory_modifier * adapter_desc.SharedSystemMemory;

            // TODO If a pc has two identical GPU'S the first one may be overwritten here
            adapter_scores[adapter_score] = adapter_desc;
        }

        return adapter_scores;
    }

    bool GB_Display::Initialize() {
        auto adapters = EnumerateAdapters();
        auto adapter_scores = DetermineDeviceScores(adapters);

        if(adapter_scores.size() == 0)
        {
            LOG(ERROR) << "Failed to enumerate adapters";
            return false;
        }

        
    }

    bool GB_Display::CreateApplicationWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = szWindowClass.c_str();
        wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

        if (!RegisterClassEx(&wcex)) {
            MessageBox(NULL,
                "Call to RegisterClassEx failed!",
                "Windows Desktop Guided Tour",
                NULL);

            return false;
        }

        // Store instance handle in our global variable
        hInst = hInstance;

        // The parameters to CreateWindowEx explained:
        // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
        // szWindowClass: the name of the application
        // szTitle: the text that appears in the title bar
        // WS_OVERLAPPEDWINDOW: the type of window to create
        // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
        // 500, 100: initial size (width, length)
        // NULL: the parent of this window
        // NULL: this application does not have a menu bar
        // hInstance: the first parameter from WinMain
        // NULL: not used in this application
        HWND hWnd = CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,
            szWindowClass.c_str(),
            szTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            500, 100,
            NULL,
            NULL,
            hInstance,
            NULL
        );

        if (!hWnd) {
            MessageBox(NULL,
                "Call to CreateWindow failed!",
                "Windows Desktop Guided Tour",
                NULL);

            return false;
        }

        // The parameters to ShowWindow explained:
        // hWnd: the value returned from CreateWindow
        // nCmdShow: the fourth parameter from WinMain
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        return true;
    }

    bool GB_Display::CreateSwapChain(HWND hwnd)
    {
    }
}
