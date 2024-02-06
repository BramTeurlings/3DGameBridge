#pragma once

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

namespace GameBridge {
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
    };

    class GB_GraphicsDevice
    {
        static const unsigned short back_buffer_count = 2;

        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<IDXGISwapChain3> swap_chain;
        ComPtr<ID3D12CommandQueue> command_queue;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        ComPtr<ID3D12Resource> render_targets[back_buffer_count];
        ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        uint32_t descriptor_size = 0;
        uint32_t frame_index = 0;

    public:
        static void CreateDXGIFactory(IDXGIFactory4** factory);
        static void GetGraphicsAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);

        // Creates device
        bool Initialize();
        bool Initialize(ID3D12Device* device, ID3D12CommandQueue* queue);

        // Create swap_chain
        bool CreateSwapChain();

        // Create swap_chain for a window
        bool CreateSwapChain(const XrSwapchainCreateInfo* createInfo, HWND hwnd);

        IDXGISwapChain1* GetSwapChain();
    };
}
