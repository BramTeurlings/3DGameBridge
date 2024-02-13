#pragma once

#include <unordered_map>
#include <array>

#include "openxr_includes.h"
#include "session.h"

XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats);
XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain);
XrResult xrDestroySwapchain(XrSwapchain swapchain);
XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images);
XrResult xrEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, XrPath* sources);
XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index);
XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo* waitInfo);
XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo);

namespace XRGameBridge {
    class GB_Display
    {
        // The main window class name.
        std::string window_class = "Game Bridge Window";

        // The string that appears in the application's title bar.
        std::string title = "XR Game Bridge";

        HWND h_wnd = 0;

    public:
        // Returns the window of the application or false if none exist
        bool CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow);
        HWND GetWindowHandle();
    };

    class GB_GraphicsDevice
    {
        static constexpr unsigned short back_buffer_count = 2;

        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<ID3D12CommandQueue> command_queue;

        ComPtr<IDXGISwapChain3> swap_chain;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        std::array<ComPtr<ID3D12Resource>, back_buffer_count> back_buffers;
        ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        uint32_t descriptor_size = 0;
        uint32_t frame_index = 0;

        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_values[back_buffer_count];
        UINT64 current_fence_value = 0;

    public:
        GB_GraphicsDevice();

        static void CreateDXGIFactory(IDXGIFactory4** factory);
        static void GetGraphicsAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);

        // Creates device
        bool Initialize();
        bool Initialize(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue);

        // Create swap_chain
        bool CreateSwapChain();

        // Create swap_chain for a window
        bool CreateSwapChain(const XrSwapchainCreateInfo* createInfo, HWND hwnd);
        std::array<ComPtr<ID3D12Resource>, back_buffer_count> GetImages();

        uint32_t GetBufferCount();
        IDXGISwapChain3* GetSwapChain();
        ID3D12Device* GetDevice();
        ID3D12CommandQueue* GetCommandQueue();
        uint32_t GetCurrentBackBufferIndex();
        void WaitForFences(const XrDuration& timeout);
    };

    inline GB_Display g_display;
    inline std::unordered_map<XrSwapchain, GB_GraphicsDevice> g_graphics_devices;
}
