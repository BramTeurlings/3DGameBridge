#include "swapchain.h"

#include <stdexcept>
#include <exception>
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

    return XR_SUCCESS;
}

XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain) {
    static uint32_t swapchain_creation_count = 1;

    // Create handle
    XrSwapchain handle = reinterpret_cast<XrSwapchain>(swapchain_creation_count);

    // Create entry in the list
    XRGameBridge::GB_ProxySwapchain& gb_proxy = XRGameBridge::g_application_render_targets[handle];

    // Create swap chain
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];

    if (gb_proxy.CreateResources(gb_session.d3d12_device, createInfo) == false) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    // Couple swap chain to the session
    *swapchain = handle;
    gb_session.swap_chain = handle;
    swapchain_creation_count++;




    //TODO Is this the right place set the session state to ready?
    //XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];
    XRGameBridge::ChangeSessionState(gb_session, XR_SESSION_STATE_READY);

    return XR_SUCCESS;
}

XrResult xrDestroySwapchain(XrSwapchain swapchain) {
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images) {
    //TODO Create actual swap chains over here

    auto& gb_render_target = XRGameBridge::g_application_render_targets[swapchain];
    uint32_t count = gb_render_target.GetBufferCount();

    *imageCountOutput = count;

    if (imageCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    if (imageCapacityInput < count) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    if (XRGameBridge::g_runtime_settings.support_d3d12) {
        if (images[0].type != XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR) {
            LOG(ERROR) << "structure type incompatible";
            return XR_ERROR_VALIDATION_FAILURE;
        }

        std::vector<XrSwapchainImageD3D12KHR> xr_images;
        auto directx_images = gb_render_target.GetBuffers();
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
    //TODO May only be called again AFTER xrReleaseSwapchainImage has been called. See specification.
    // return XR_ERROR_CALL_ORDER_INVALID

    auto& gb_proxy = XRGameBridge::g_application_render_targets[swapchain];
    XrResult res = gb_proxy.AcquireNextImage(*index);
    return res;
}

XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo* waitInfo) {
    //TODO see specification for other waiting requirements

    uint64_t wait_duration = INFINITE;
    if (waitInfo->timeout != XR_INFINITE_DURATION) {
        wait_duration = waitInfo->timeout;
    }

    auto& gb_proxy = XRGameBridge::g_application_render_targets[swapchain];

    return gb_proxy.WaitForImage(wait_duration);
}

XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo) {
    // Basically tells the runtime that the application is done with an image

    auto& gb_proxy = XRGameBridge::g_application_render_targets[swapchain];
    return gb_proxy.ReleaseImage();
}

namespace XRGameBridge {


    bool GB_ProxySwapchain::CreateResources(const ComPtr<ID3D12Device>& device, const XrSwapchainCreateInfo* createInfo) {
        for (uint32_t i = 0; i < g_back_buffer_count; i++) {
            // Describe and create a Texture2D.
            D3D12_RESOURCE_DESC textureDesc = {};
            textureDesc.MipLevels = 1;
            textureDesc.Format = static_cast<DXGI_FORMAT>(createInfo->format);
            textureDesc.Width = createInfo->width;
            textureDesc.Height = createInfo->height;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            textureDesc.DepthOrArraySize = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            //D3D12_DEPTH_STENCIL_VALUE depth_stencil_value;
            //depth_stencil_value.Depth = 100.f;
            //depth_stencil_value.Stencil = 0;

            float clear_color[4]{ 0.851f, 0.42f, 0.09f, 1.0f };

            D3D12_CLEAR_VALUE clear_value{
                static_cast<DXGI_FORMAT>(createInfo->format),
                0.5f
            };

            auto resource = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(device->CreateCommittedResource(
                &resource,
                D3D12_HEAP_FLAG_NONE,
                &textureDesc,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                &clear_value,
                IID_PPV_ARGS(&back_buffers[i])));
        }

        // Create descriptor heaps.
        {
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = g_back_buffer_count;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtv_heap)))) {
                LOG(ERROR) << "Failed to create d3d12 rtv descriptor heap";
                return false;
            }

            // TODO we create an srv heap here but not srv's themselves later on
            // Describe and create a shader resource view (SRV) heap for the texture.
            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
            srvHeapDesc.NumDescriptors = g_back_buffer_count;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srv_heap)))) {
                LOG(ERROR) << "Failed to create d3d12 srv descriptor heap";
                return false;
            }
        }

        rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        cbc_srv_uav_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // Create descriptors
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());
            CD3DX12_CPU_DESCRIPTOR_HANDLE srv_handle(srv_heap->GetCPUDescriptorHandleForHeapStart());

            for (int32_t i = 0; i < g_back_buffer_count; i++) {
                //std::wstringstream ss; ss << "Swap Container Resource: " << i;
                //back_buffers[i]->SetName(ss.str().c_str());

                // Create a RTV for each frame.
                device->CreateRenderTargetView(back_buffers[i].Get(), nullptr, rtv_handle);
                rtv_handle.Offset(1, rtv_descriptor_size);

                // Create SRV for each frame
                device->CreateShaderResourceView(back_buffers[i].Get(), nullptr, srv_handle);
                srv_handle.Offset(1, cbc_srv_uav_descriptor_size);
            }
        }

        // Create fence
        device->CreateFence(fence_values[current_frame_index], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        // Create an event handle to use for frame synchronization.
        fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fence_event == nullptr) {
            HRESULT_FROM_WIN32(GetLastError());
            return false;
        }

        return true;
    }

    void GB_ProxySwapchain::DestroyResources() {
        for (int32_t i = 0; i < g_back_buffer_count; i++) {
            back_buffers[i].Reset();
        }

        rtv_heap.Reset();
        srv_heap.Reset();
    }

    uint32_t GB_ProxySwapchain::GetBufferCount() {
        return g_back_buffer_count;
    }

    std::array<ComPtr<ID3D12Resource>, g_back_buffer_count> GB_ProxySwapchain::GetBuffers() {
        return back_buffers;
    }

    ComPtr<ID3D12DescriptorHeap>& GB_ProxySwapchain::GetRtvHeap() {
        return rtv_heap;
    }

    ComPtr<ID3D12DescriptorHeap>& GB_ProxySwapchain::GetSrvHeap() {
        return srv_heap;
    }

    XrResult GB_ProxySwapchain::AcquireNextImage(uint32_t& index) {
        if (current_image_state != IMAGE_STATE_RELEASED) {
            return XR_ERROR_CALL_ORDER_INVALID;
        }

        // set current frame values to the values of the next frame
        current_frame_index++;
        current_frame_index = current_frame_index % g_back_buffer_count;
        index = current_frame_index;
        current_image_state = IMAGE_STATE_ACQUIRED;
        return XR_SUCCESS;
    }

    // Wait for the gpu to be done with the image so we can use it for drawing
    // Must only be called after GetCurrentBackBufferIndex to be sure that the image index is not in use anymore
    XrResult GB_ProxySwapchain::WaitForImage(const XrDuration& timeout) {
        if (current_image_state != IMAGE_STATE_ACQUIRED) {
            return XR_ERROR_CALL_ORDER_INVALID;
        }

        // Should always be called AFTER GetCurrentBackBufferIndex. So GetCompletedValue van be compared to the new frame fence value.
        uint64_t completed_value = fence->GetCompletedValue();
        if (completed_value < fence_values[current_frame_index]) {
            // Fire event on completion
            fence->SetEventOnCompletion(fence_values[current_frame_index], fence_event);
            // Wait for the fence to be signaled and fire the event
            HRESULT res = WaitForSingleObjectEx(fence_event, ch::duration_cast<ch::milliseconds>(ch::nanoseconds(timeout)).count(), FALSE);
            if (res == WAIT_TIMEOUT) {
                return XR_TIMEOUT_EXPIRED;
            }
        }

        // TODO wait for fence -> transition image -> set event on the same fence -> wait again on the same fence
        // This is so we can guarantee that the image is free and in the correct state to be used by the application
        //TransitionBackBufferImage(COMMAND_RESOURCE_INDEX_TRANSITION, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        // Remark: fence value will be incremented until the swapchain is destroyed. 
        fence_values[current_frame_index]++;

        // Set the image state to render target because we have waited for the image to be freed so it can be used by the application again.
        current_image_state = IMAGE_STATE_RENDER_TARGET;

        return XR_SUCCESS;
    }

    XrResult GB_ProxySwapchain::ReleaseImage() {
        // TODO Should release the oldest image in the swap chain according to the spec, this already happens implicitly when acquiring images from the swap chain.
        // When Acquiring, an image is released that has already been presented, that index is then used to render a new image to.

        // Image state must be IMAGE_STATE_RENDER_TARGET before calling this. Image must have waited without timeout.
        if (current_image_state != IMAGE_STATE_RENDER_TARGET) {
            return XR_ERROR_CALL_ORDER_INVALID;
        }

        // TODO moved this call to compositor for now
        // Schedule a Signal command in the queue. for the currently rendered frame
        //command_queue->Signal(fence.Get(), fence_values[current_frame_index]);


        /// TODO (ONLY FOR SETTING TO IMAGE_STATE_WEAVING) test if this works with multi threaded rendering applications. It could be that after release, another thread will immediately call AcquireSwapchainImage, which will now return CALL_ORDER_INVALID since the weaving still has to happen on the first thread.
        // Set the image state to IMAGE_STATE_RELEASED. After this the image can be weaved. The image can also be reacquired by the application though.
        // TODO Set up the fences in a way that WaitForImage should also wait for the presentation to be done
        current_image_state = IMAGE_STATE_RELEASED;

        return XR_SUCCESS;
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

    bool GB_GraphicsDevice::CreateSwapChain(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue, const XrSwapchainCreateInfo* createInfo, HWND hwnd) {
        // TODO On failure all objects here should be destroyed
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
        CreateDXGIFactory(&factory);

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = createInfo->width;
        swapChainDesc.Height = createInfo->height;
        swapChainDesc.Format = static_cast<DXGI_FORMAT>(createInfo->format);
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
        swapChainDesc.BufferCount = g_back_buffer_count;
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
        if (FAILED(factory->CreateSwapChainForHwnd(queue.Get(), hwnd, &swapChainDesc, &fsSwapChainDesc, nullptr, &swapChain))) {
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
            rtvHeapDesc.NumDescriptors = g_back_buffer_count;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)))) {
                LOG(ERROR) << "Failed to create d3d12 rtv descriptor heap";
                return false;
            }

            // TODO we create an srv heap here but not srv's themselves later on
            // Describe and create a shader resource view (SRV) heap for the texture.
            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
            srvHeapDesc.NumDescriptors = g_back_buffer_count;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)))) {
                LOG(ERROR) << "Failed to create d3d12 srv descriptor heap";
                return false;
            }

            rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            for (int32_t i = 0; i < g_back_buffer_count; i++) {
                if (FAILED(swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffers[i])))) {
                    LOG(ERROR) << "Failed to create rtv";
                    return false;
                }
                std::wstringstream ss; ss << "Swapchain Buffer " << i;
                back_buffers[i]->SetName(ss.str().c_str());
                device->CreateRenderTargetView(back_buffers[i].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(1, rtv_descriptor_size);
            }
        }

        return true;
    }

    std::array<ComPtr<ID3D12Resource>, g_back_buffer_count> GB_GraphicsDevice::GetImages() {
        return back_buffers;
    }

    ComPtr<ID3D12DescriptorHeap>& GB_GraphicsDevice::GetRtvHeap() {
        return m_rtvHeap;
    }

    ComPtr<ID3D12DescriptorHeap>& GB_GraphicsDevice::GetSrvHeap() {
        return m_srvHeap;
    }

    uint32_t GB_GraphicsDevice::GetRtvDescriptorSize() {
        return rtv_descriptor_size;
    }

    uint32_t GB_GraphicsDevice::AcquireNextImage() {
        // TODO get image index from the swapchain
        return swap_chain->GetCurrentBackBufferIndex();
    }

    // Called from xrEndFrame, cause then we know the application is done with rendering this image.
    void GB_GraphicsDevice::PresentFrame() {
        // TODO Transitioning images state without waiting on the queue to finish, not sure this will break eventually. Maybe dx12 is synchronizing implicitly?
        //TransitionBackBufferImage(COMMAND_RESOURCE_INDEX_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        swap_chain->Present(1, 0);


        // barrier to render target
    }
}
