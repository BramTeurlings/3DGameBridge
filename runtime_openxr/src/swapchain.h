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
    enum ImageState {
        IMAGE_STATE_WAITING,
        IMAGE_STATE_RELEASED,

        IMAGE_STATE_ACQUIRED,
        IMAGE_STATE_RENDER_TARGET,
        IMAGE_STATE_WEAVING,
        IMAGE_STATE_DONE_WEAVING
    };

    class GB_Display {
        // The main window class name.
        std::string window_class = "Game Bridge Window";

        // The string that appears in the application's title bar.
        std::string title = "XR Game Bridge";

        HWND h_wnd = 0;

    public:
        // Returns the window of the application or false if none exist
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        bool CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow);
        HWND GetWindowHandle();
        void UpdateWindow();
    };

    // Back buffer count
    static constexpr unsigned short back_buffer_count = 2;

    // TODO Use resources instead of creating multiple swap chains? Is that better?
    // UEVR create a lot of swap chains so let's just use images....
    class GB_ProxySwapchain {
        ComPtr<ID3D12CommandQueue> command_queue;
        std::array<ComPtr<ID3D12Resource>, back_buffer_count> back_buffers;
        ComPtr<ID3D12DescriptorHeap> rtv_heap;
        ComPtr<ID3D12DescriptorHeap> srv_heap;

        uint32_t rtv_descriptor_size = 0;
        uint32_t cbc_srv_uav_descriptor_size = 0;

        uint32_t current_frame_index = 0;
        ImageState current_image_state = IMAGE_STATE_RENDER_TARGET;
        UINT64 previous_fence_value = 0;

        // TODO We are using fences for every image instead of every frame, test if we can use fences per frame only instead
        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_values[back_buffer_count];

    public:
        GB_ProxySwapchain(GB_ProxySwapchain& other) = delete;
        GB_ProxySwapchain(GB_ProxySwapchain&& other) = delete;

        bool CreateResources(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue, const XrSwapchainCreateInfo* createInfo);
        bool DestroyResources();

        // Returns the oldest image index
        bool AcquireNextImage(uint32_t& index);

        // Waits for an image that has been weaved
        XrResult WaitForImage(const XrDuration& timeout);

        // Make the image available for weaving
        bool ReleaseImage();
    };

    class GB_GraphicsDevice {
        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<ID3D12CommandQueue> command_queue;

        ComPtr<IDXGISwapChain3> swap_chain;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        std::array<ComPtr<ID3D12Resource>, back_buffer_count> back_buffers;

        uint32_t rtv_descriptor_size = 0;
        uint32_t frame_index = 0;

        // TODO Use command allocator and lists to transition all images at the same time in an external function
        enum CommandResourceIndex { COMMAND_RESOURCE_INDEX_TRANSITION, COMMAND_RESOURCE_INDEX_PRESENT };
        std::array<ComPtr<ID3D12CommandAllocator>, 2> command_allocators;
        std::array<ComPtr<ID3D12GraphicsCommandList>, 2 > command_lists;

        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_values[back_buffer_count];

        UINT64 current_fence_value = 0;
        ImageState current_image_state = IMAGE_STATE_RELEASED;

    public:
        static void CreateDXGIFactory(IDXGIFactory4** factory);
        static void GetGraphicsAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);

        // Creates device
        bool Initialize();
        bool Initialize(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue);

        // TODO swapchain is only necessary if we render to the XR Game Bridge window, otherwise we render to the back buffer of UEVR window
        bool CreateSwapChain(const XrSwapchainCreateInfo* createInfo, HWND hwnd);
        std::array<ComPtr<ID3D12Resource>, back_buffer_count> GetImages();

        // TODO can probably all be removed
        uint32_t GetBufferCount();
        IDXGISwapChain3* GetSwapChain();
        ID3D12Device* GetDevice();
        ID3D12CommandQueue* GetCommandQueue();
        uint32_t GetCurrentBackBufferIndex();
        XrResult WaitForFences(const XrDuration& timeout);
        XrResult ReleaseSwapchainImage();
        // ~////////////

        void AcquireNextImage();
        void ComposeImage();
        void Present();

        void TransitionBackBufferImage(CommandResourceIndex index, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after);

        void PresentFrame();
    };

    void SetToRenderTarget();

    inline GB_Display g_display;
    inline std::unordered_map<XrSwapchain, GB_ProxySwapchain> g_application_render_targets;

    inline std::unordered_map<XrSwapchain, GB_GraphicsDevice> g_graphics_devices;
}
