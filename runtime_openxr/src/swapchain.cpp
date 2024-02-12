#include "swapchain.h"

#include <stdexcept>
#include <vector>

#include <wrl/client.h>

#include "easylogging++.h"
#include "openxr_functions.h"
#include "instance.h"
#include "settings.h"
#include "system.h"

XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats) {
    XRGameBridge::GraphicsBackend backend;

    try {
        XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions.at(session);
        XRGameBridge::GB_System& gb_system = XRGameBridge::g_systems.at(gb_session.system);
        backend = gb_system.active_graphics_backend;
    }
    catch (std::out_of_range& e) {
        return XR_ERROR_SYSTEM_INVALID;
    }
    catch (std::exception& e) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    std::vector<int64_t> supported_swapchain_formats;
    if (backend == XRGameBridge::GraphicsBackend::D3D12) {
        supported_swapchain_formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
        supported_swapchain_formats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    }
    if (backend == XRGameBridge::GraphicsBackend::D3D11) {
        // not implemented
        return XR_ERROR_RUNTIME_FAILURE;
    }

    uint32_t count = supported_swapchain_formats.size();
    *formatCountOutput = count;

    if (formatCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    if (formatCapacityInput < count) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    // Fill array
    memcpy_s(formats, formatCapacityInput * sizeof(int64_t), supported_swapchain_formats.data(), supported_swapchain_formats.size() * sizeof(int64_t));

    //TODO Is this the right place set the session state to ready?
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];
    XRGameBridge::ChangeSessionState(gb_session, XR_SESSION_STATE_READY);

    return XR_SUCCESS;
}

XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain) {
    static uint32_t swapchain_creation_count = 1;
    // Create window
    XRGameBridge::g_display.CreateApplicationWindow(XRGameBridge::g_runtime_settings.hInst, true);
    XrSwapchain handle = reinterpret_cast<XrSwapchain>(swapchain_creation_count);

    // Create entry in the list
    XRGameBridge::GB_GraphicsDevice& graphics_device = XRGameBridge::g_graphics_devices[handle];

    // Create swap chain
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];
    graphics_device.Initialize(gb_session.d3d12_device, gb_session.command_queue);
    if(graphics_device.CreateSwapChain(createInfo, XRGameBridge::g_display.GetWindowHandle()))
    {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    *swapchain = handle;
    swapchain_creation_count++;
    return XR_SUCCESS;
}

XrResult xrDestroySwapchain(XrSwapchain swapchain) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images) {
    auto gb_graphics_device = XRGameBridge::g_graphics_devices[swapchain];
    uint32_t count = gb_graphics_device.GetBufferCount();

    *imageCountOutput = count;

    if (imageCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    if (imageCapacityInput < count) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    if (XRGameBridge::g_runtime_settings.support_d3d12) {
        if(images[0].type != XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR)
        {
            LOG(ERROR) << "structure type incompatible";
            return XR_ERROR_VALIDATION_FAILURE;
        }

        std::vector<XrSwapchainImageD3D12KHR> xr_images;
        auto directx_images = gb_graphics_device.GetImages();
        for (uint32_t i = 0; i < count; i++) {
            XrSwapchainImageD3D12KHR image{};
            image.texture = directx_images[i].Get();
            xr_images.push_back(image);
        }

        // Fill array
        memcpy_s(images, imageCapacityInput * sizeof(XrSwapchainImageD3D12KHR), xr_images.data(), xr_images.size() * sizeof(XrSwapchainImageD3D12KHR));
        return XR_SUCCESS;
    }
}

XrResult xrEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, XrPath* sources) {
    LOG(INFO) << "Called " << __func__;
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index) {
    LOG(INFO) << "Called " << __func__;
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo* waitInfo) {
    LOG(INFO) << "Called " << __func__;
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo) {
    LOG(INFO) << "Called " << __func__;
    return XR_ERROR_RUNTIME_FAILURE;
}

namespace XRGameBridge {
    void MessageLoop() {
        // Start the message loop.

        // Main message loop:
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

        return 0;
    }

    bool GB_Display::CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow) {
        WNDCLASSEX window_ex;

        window_ex.cbSize = sizeof(WNDCLASSEX);
        window_ex.style = CS_HREDRAW | CS_VREDRAW;
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

        // Create window
        h_wnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, window_class.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, hInstance, NULL);
        if (!h_wnd) {
            MessageBox(NULL, "Call to CreateWindow failed!", "XR Game Bridge", NULL);
            return false;
        }

        // The parameters to ShowWindow explained:
        // h_wnd: the value returned from CreateWindow
        // nCmdShow: the fourth parameter from WinMain
        ShowWindow(h_wnd, nCmdShow);
        UpdateWindow(h_wnd);

        return true;
    }

    HWND GB_Display::GetWindowHandle() {
        return h_wnd;
    }

    void GB_GraphicsDevice::CreateDXGIFactory(IDXGIFactory4** factory) {
        // Create a DXGIFactory object.
        UINT dxgi_factory_flags = 0;
        HRESULT err = CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(factory));
        if (FAILED(err)) {
            LOG(ERROR) << "Could not create DXGIFactory with error: " << err;
        }
    }

    void GB_GraphicsDevice::GetGraphicsAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter) {
        *ppAdapter = nullptr;

        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
            for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter))); ++adapterIndex) {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                    // Don't select the Basic Render Driver adapter.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                    break;
                }
            }
        }

        if (adapter.Get() == nullptr) {
            for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex) {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                    // Don't select the Basic Render Driver adapter.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }

    bool GB_GraphicsDevice::Initialize() {
        // Create device
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
        CreateDXGIFactory(&factory);
        GetGraphicsAdapter(factory.Get(), &hardwareAdapter, true);

        if (D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12_device))) {
            LOG(ERROR) << "Failed creating d3d12 device";
            return false;
        }

        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (d3d12_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue))) {
            LOG(ERROR) << "Failed creating d3d12 command queue";
            return false;

        }

        return true;
    }

    bool GB_GraphicsDevice::Initialize(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue) {
        d3d12_device = device;
        command_queue = queue;
        return true;
    }

    bool GB_GraphicsDevice::CreateSwapChain() {

        return false;
    }

    bool GB_GraphicsDevice::CreateSwapChain(const XrSwapchainCreateInfo* createInfo, HWND hwnd) {
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
        CreateDXGIFactory(&factory);

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = createInfo->width;
        swapChainDesc.Height = createInfo->height;
        swapChainDesc.Format = static_cast<DXGI_FORMAT>(createInfo->format);
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = back_buffer_count;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;


        // Swap chain needs the queue so that it can force a flush on it.
        ComPtr<IDXGISwapChain1> swapChain;
        if (FAILED(factory->CreateSwapChainForHwnd(command_queue.Get(), hwnd, &swapChainDesc, &fsSwapChainDesc, nullptr, &swapChain))) {
            LOG(ERROR) << "Failed to create d3d12 swap chain";
            return false;
        }

        if (FAILED(swapChain.As(&swap_chain))) {
            LOG(ERROR) << "Failed to get ComPtr object";
            return false;
        }

        frame_index = swap_chain->GetCurrentBackBufferIndex();

        // Create descriptor heaps.
        {
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = back_buffer_count;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            if (FAILED(d3d12_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)))) {
                LOG(ERROR) << "Failed to create d3d12 rtv descriptor heap";
                return false;
            }

            // Describe and create a shader resource view (SRV) heap for the texture.
            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
            srvHeapDesc.NumDescriptors = 1;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (FAILED(d3d12_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)))) {
                LOG(ERROR) << "Failed to create d3d12 srv descriptor heap";
                return false;
            }

            descriptor_size = d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            for (uint32_t i = 0; i < back_buffer_count; i++) {
                if (FAILED(swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffers[i])))) {
                    LOG(ERROR) << "Failed to create rtv";
                    return false;
                }
                d3d12_device->CreateRenderTargetView(back_buffers[i].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(i, descriptor_size);
            }
        }

        if (FAILED(d3d12_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)))) {
            LOG(ERROR) << "Failed to create d3d12 command allocator";
            return false;
        }

        return false;
    }

    std::array<ComPtr<ID3D12Resource>, GB_GraphicsDevice::back_buffer_count> GB_GraphicsDevice::GetImages()
    {
        return back_buffers;
    }

    uint32_t GB_GraphicsDevice::GetBufferCount() {
        return back_buffer_count;
    }

    IDXGISwapChain1* GB_GraphicsDevice::GetSwapChain() {
        return swap_chain.Get();
    }
}
