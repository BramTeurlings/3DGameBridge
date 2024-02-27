#include "session.h"

#include <stdexcept>

#include "easylogging++.h"
#include "openxr_functions.h"
#include "instance.h"
#include "system.h"
#include "settings.h"
#include "compositor.h"
#include "swapchain.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session) {
    // TODO refactor local scope static variables
    static uint64_t session_creation_count = 1;

    try {
        XRGameBridge::GB_System& system = XRGameBridge::g_systems.at(createInfo->systemId);
        if (!system.features_enumerated) {
            return XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING;
        }

        if (system.instance != instance) {
            return XR_ERROR_SYSTEM_INVALID;
        }
    }
    catch (std::out_of_range& e) {
        return XR_ERROR_SYSTEM_INVALID;
    }
    catch (std::exception& e) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    // Create entry if it doesn't exist
    // Note: This means it overwrite all except for the id member it if it does exist
    XrSession handle = reinterpret_cast<XrSession>(session_creation_count);
    XRGameBridge::GB_Session& new_session = XRGameBridge::g_sessions[handle];

    // Initialize session with state idle
    new_session.id = handle;
    new_session.instance = instance;
    new_session.system = createInfo->systemId;
    new_session.session_state = XR_SESSION_STATE_IDLE;
    new_session.session_epoch = std::chrono::high_resolution_clock::now();

    // DirectX 12
    if (XRGameBridge::g_runtime_settings.support_d3d12) {
        const XrGraphicsBindingD3D12KHR* d3d12_bindings = static_cast<const XrGraphicsBindingD3D12KHR*> (createInfo->next);
        new_session.d3d12_device = d3d12_bindings->device;
        new_session.command_queue = d3d12_bindings->queue;
    }

    *session = handle;
    session_creation_count++;

    // TODO Not sure where to put the compositor, it has to be initialized by the session, but you render to a system
    // Maybe a system should own a compositor, but it is created and destroyed by the client?
    new_session.compositor.Initialize(new_session.d3d12_device, new_session.command_queue, 2);

    return XR_SUCCESS;
}

XrResult xrDestroySession(XrSession session) {
    // TODO Should probably destroy all objects related to a session.
    // If the system will hold the device and command queues, it should delete the system too!
    // That's very unwanted. So let's not do that
    // Swap chains depend on the session since it's holds the device and command queue, so swap chains should be destroyed on session destroy.
    // Also action sets/g_actions attached to the session should be destroyed
    LOG(INFO) << "Called " << __func__; return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrBeginSession(XrSession session, const XrSessionBeginInfo* beginInfo) {

    // TODO check if view configuration type is supported
    // TODO, move SESSION_READY logic to here, check here whether all components are initialized for the session to be put on READY.

    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];
    if (gb_session.session_state == XR_SESSION_STATE_IDLE) {
        LOG(ERROR) << "Session not ready";
        return XR_ERROR_SESSION_NOT_READY;
    }
    if (gb_session.session_state != XR_SESSION_STATE_READY) {
        LOG(ERROR) << "Session is already running";
        return XR_ERROR_SESSION_RUNNING;
    }

    gb_session.view_configuration = beginInfo->primaryViewConfigurationType;

    XRGameBridge::ChangeSessionState(gb_session, XR_SESSION_STATE_FOCUSED);

    gb_session.display.CreateApplicationWindow(XRGameBridge::g_runtime_settings.hInst, true);


    auto vectori = XRGameBridge::GetDummyScreenResolution();

    XrSwapchainCreateInfo swapchain_info;
    swapchain_info.width = vectori.x;
    swapchain_info.height = vectori.y;
    swapchain_info.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    gb_session.window_swapchain.CreateSwapChain(gb_session.d3d12_device, gb_session.command_queue ,&swapchain_info, gb_session.display.GetWindowHandle());

    return XR_SUCCESS;
}

XrResult xrEndSession(XrSession session) {
    LOG(INFO) << "Called " << __func__;
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrRequestExitSession(XrSession session) {
    LOG(INFO) << "Called " << __func__;
    return XR_ERROR_RUNTIME_FAILURE;
}

// TODO Use frame display time as frame ids
XrResult xrWaitFrame(XrSession session, const XrFrameWaitInfo* frameWaitInfo, XrFrameState* frameState) {
    // TODO simple implementation so the application can continue. Should when I understand this part better
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];
    bool should_wait = true;

    // Blocking wait, blocks until BeginFrame was called
    while (should_wait) {
        if (gb_session.wait_frame_state_mutex.try_lock()) {
            if (gb_session.wait_frame_state == XRGameBridge::NewFrameAllowed) {
                gb_session.wait_frame_state = XRGameBridge::NewFrameBusy;
                break;
            }
        }
        gb_session.wait_frame_state_mutex.unlock();
        std::this_thread::sleep_for(ch::nanoseconds(10));
    }

    gb_session.wait_frame_state_mutex.unlock();

    // Time point since session epoch + 16 milliseconds
    // Super simple version of this for now I guess

    /* As far as I understand:
     * predictedDisplayTime: The future time point the next image will be displayed at
     * predictedDisplayPeriod: The amount of time the next image will be visible (presented) on the screen 
     */

    // 1/60th in nanoseconds
    uint32_t nanoseconds = 1.0 / 60.0 * 1000 * 1000 * 1000;
    auto refresh_rate = ch::nanoseconds(nanoseconds);
    // Image should be displayed for the <refresh rate> amount of time
    auto display_period = ch::nanoseconds(refresh_rate);
    // Time since the epoch the application is running now, add the refresh rate to predict the time the next image will be displayed.
    auto display_time = ch::high_resolution_clock::now() - gb_session.session_epoch + refresh_rate;

    frameState->predictedDisplayPeriod = display_period.count();
    frameState->predictedDisplayTime = display_time.count();
    frameState->shouldRender = true;

    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}

XrResult xrBeginFrame(XrSession session, const XrFrameBeginInfo* frameBeginInfo) {
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];

    std::lock_guard guard(gb_session.wait_frame_state_mutex);
    if (gb_session.wait_frame_state != XRGameBridge::NewFrameBusy) {
        return XR_ERROR_CALL_ORDER_INVALID;
    }

    gb_session.wait_frame_state = XRGameBridge::FrameState::NewFrameAllowed;

    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}

XrResult xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo) {
    // TODO If no layers are provided then the display must be cleared.
    // Present the frame for session
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];

    // TODO Don't want to keep swapchains in the swapchain anymore, either move them to the compositor, or the system.
    auto& gb_graphics_device = gb_session.window_swapchain;
    int32_t index = gb_graphics_device.AcquireNextImage();
    auto& gb_compositor = gb_session.compositor;
    auto& cmd_list = gb_compositor.GetCommandList(index);
    auto& cmd_allocator = gb_compositor.GetCommandAllocator(index);

    // Prepare command list // TODO set pipeline state when resetting command list later
    cmd_allocator->Reset();
    cmd_list->Reset(cmd_allocator.Get(), gb_compositor.GetPipelineState().Get());

    // TODO transition proxy images to unordered access/shader source (If I'm right...)
    gb_compositor.TransitionImage(cmd_list.Get(), gb_graphics_device.GetImages()[index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Set render target to the swap chain resource
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(gb_graphics_device.GetRtvHeap()->GetCPUDescriptorHandleForHeapStart(), index, gb_graphics_device.GetRtvDescriptorSize());
    cmd_list->OMSetRenderTargets(1, &rtvHandle, true, nullptr);

    const auto screen_vector = XRGameBridge::GetDummyScreenResolution();
    const float width = static_cast<float>(screen_vector.x);
    const float height = static_cast<float>(screen_vector.y);
    D3D12_VIEWPORT view_port{ 0, 0, width, height, 0.0f, 1.0f };
    D3D12_RECT scissor_rect {0, 0, screen_vector.x, screen_vector .y};

    cmd_list->RSSetViewports(1, &view_port);
    cmd_list->RSSetScissorRects(1, &scissor_rect);


    float clear_color[4] = {0,0,1,1};
   // cmd_list->ClearRenderTargetView(rtvHandle, clear_color, 0, nullptr);
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    //auto layer = reinterpret_cast<const XrCompositionLayerProjection*>(frameEndInfo->layers[0]);
    //auto& gb_proxy = XRGameBridge::g_application_render_targets[layer->views[0].subImage.swapchain];
    //cmd_list->CopyResource(gb_graphics_device.GetImages()[index].Get(), gb_proxy.GetBuffers()[index].Get());


    // TODO copy image for now instead of composing layers
    //// Compose and draw to the render target
    gb_compositor.ComposeImage(frameEndInfo, cmd_list.Get());



    // TODO transition proxy images back to render target
    gb_compositor.TransitionImage(cmd_list.Get(), gb_graphics_device.GetImages()[index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    // Close command list
    cmd_list->Close();

    // Execute command lists
    gb_compositor.ExecuteCommandLists(cmd_list.Get(), frameEndInfo);

    // Present to window
    gb_graphics_device.PresentFrame();

    // TODO Debug layers complaining that the initialized resource clear color is mismatching ClearRenderTargetViewCall because the application calls it.
    // TODO Clear resources here

    // TODO Figure out how to use all the layers and views
    // TODO create function inside the actual swapchain render to the window


    // Update window
    gb_session.display.UpdateWindow();

    return XR_SUCCESS;
}

void XRGameBridge::ChangeSessionState(GB_Session& session, XrSessionState state) {
    session.session_state = state;
    EventManager& event_manager = g_game_bridge_instance->GetEventManager();

    event_manager.PrepareForEventStreamWriting();
    if (state == XR_SESSION_STATE_READY) {
        XrEventDataSessionStateChanged state_change;
        state_change.type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED;
        state_change.session = session.id;
        state_change.state = XR_SESSION_STATE_READY;
        state_change.time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - session.session_epoch).count();
        g_openxr_event_stream_writer->SubmitEvent(XR_SESSION_STATE_READY, sizeof(XrEventDataSessionStateChanged), &state_change);
        event_manager.PrepareForEventStreamReading();// TODO FOR DEBUG PURPOSES SHOULD BE REMOVED ASAP
    }
    else if (state == XR_SESSION_STATE_FOCUSED) {
        XrEventDataSessionStateChanged state_change;
        state_change.type = XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED;
        state_change.session = session.id;
        state_change.state = XR_SESSION_STATE_FOCUSED;
        state_change.time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - session.session_epoch).count();
        g_openxr_event_stream_writer->SubmitEvent(XR_SESSION_STATE_FOCUSED, sizeof(XrEventDataSessionStateChanged), &state_change);
        event_manager.PrepareForEventStreamReading();// TODO FOR DEBUG PURPOSES SHOULD BE REMOVED ASAP
    }
}
