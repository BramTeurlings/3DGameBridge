#pragma once

#include <unordered_map>
#include <array>

#include "openxr_includes.h"

XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats);
XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain);
XrResult xrDestroySwapchain(XrSwapchain swapchain);
XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images);
XrResult xrEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, XrPath* sources);
XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index);
XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo* waitInfo);
XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo);

namespace XRGameBridge {
    // Forward declaration for GB_ProxySwapchain friend
    class GB_Compositor;

    enum ImageState {
        IMAGE_STATE_WAITING,
        IMAGE_STATE_RELEASED,

        IMAGE_STATE_ACQUIRED,
        IMAGE_STATE_RENDER_TARGET,
        IMAGE_STATE_WEAVING,
        IMAGE_STATE_DONE_WEAVING
    };

    // Back buffer count
    //TODO make this const inside the class and mutable through the constructor
    constexpr unsigned short g_back_buffer_count = 2;

    // TODO Use resources instead of creating multiple swap chains? Is that better?
    // UEVR create a lot of swap chains so let's just use images....
    class GB_ProxySwapchain {
        friend GB_Compositor;

        //ComPtr<ID3D12CommandQueue> command_queue;
        std::array<ComPtr<ID3D12Resource>, g_back_buffer_count> back_buffers;
        ComPtr<ID3D12DescriptorHeap> rtv_heap;
        ComPtr<ID3D12DescriptorHeap> srv_heap;

        uint32_t rtv_descriptor_size = 0;
        uint32_t cbc_srv_uav_descriptor_size = 0;

        uint32_t current_frame_index = 0;
        ImageState current_image_state = IMAGE_STATE_RELEASED;
        UINT64 previous_fence_value = 0;

        // TODO We are using fences for every image instead of every frame, test if we can use fences per frame only instead
        HANDLE fence_event;
        ComPtr<ID3D12Fence> fence;
        UINT64 fence_values[g_back_buffer_count];

    public:
        GB_ProxySwapchain() = default;
        GB_ProxySwapchain(GB_ProxySwapchain& other) = delete;
        GB_ProxySwapchain(GB_ProxySwapchain&& other) = delete;

        bool CreateResources(const ComPtr<ID3D12Device>& device, const XrSwapchainCreateInfo* createInfo);
        void DestroyResources();

        uint32_t GetBufferCount();
        std::array<ComPtr<ID3D12Resource>, g_back_buffer_count> GetBuffers();
        ComPtr<ID3D12DescriptorHeap>& GetRtvHeap();
        ComPtr<ID3D12DescriptorHeap>& GetSrvHeap();

        // Returns the oldest image index
        XrResult AcquireNextImage(uint32_t& index);

        // Waits for an image that has been weaved
        XrResult WaitForImage(const XrDuration& timeout);

        // Make the image available for weaving
        XrResult ReleaseImage();
    };

    // TODO swapchain is only necessary if we render to the XR Game Bridge window, otherwise we render to the back buffer of UEVR window
    // TODO Remark, this swapchain does not have synchronization objects, this is because we already wait for fences on proxy swapchains, which implicitly waits for this swapchains resources.
    // TODO Make a render loop for presentation to the window/UEVR that uses it's own fences. Then proxy swapchains don't need their own fences anymore.
    class GB_GraphicsDevice {
        ComPtr<IDXGISwapChain3> swap_chain;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        std::array<ComPtr<ID3D12Resource>, g_back_buffer_count> back_buffers;

        uint32_t rtv_descriptor_size = 0;
        uint32_t frame_index = 0;

    public:
        static void CreateDXGIFactory(IDXGIFactory4** factory);
        static void GetGraphicsAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);

        // Creates device
        bool CreateSwapChain(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue, const XrSwapchainCreateInfo* createInfo, HWND hwnd);

        std::array<ComPtr<ID3D12Resource>, g_back_buffer_count> GetImages();
        ComPtr<ID3D12DescriptorHeap>& GetRtvHeap();
        ComPtr<ID3D12DescriptorHeap>& GetSrvHeap();
        uint32_t GetRtvDescriptorSize();

        uint32_t AcquireNextImage();
        void PresentFrame();
    };

    inline std::unordered_map<XrSwapchain, GB_ProxySwapchain> g_proxy_swapchains;
    //inline std::unordered_map<XrSwapchain, GB_GraphicsDevice> g_graphics_devices;
}
