#include "session.h"

#include <stdexcept>

#include "easylogging++.h"
#include "openxr_functions.h"
#include "instance.h"
#include "system.h"
#include "settings.h"

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
    //frameEndInfo->layers

    // Present the frame for session
    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];
    XRGameBridge::GB_GraphicsDevice& gb_graphics_device = XRGameBridge::g_graphics_devices[gb_session.swap_chain];

    if(frameEndInfo->layerCount == 0)
    {
        // TODO clear the screen when no layers are present
    }

    for(uint32_t i = 0; i < frameEndInfo->layerCount; i++)
    {
        auto layer = reinterpret_cast<const XrCompositionLayerProjection*>( frameEndInfo->layers[i]);

        // TODO use alpha bits
        layer->layerFlags& XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        layer->layerFlags& XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT;


    }

    //gb_graphics_device.PresentFrame();

    // TODO Debug layers complaining that the initialized resource clear color is mismatching ClearRenderTargetViewCall because the application calls it.
    // TODO Clear resources here

    // TODO Figure out how to use all the layers and views
    // TODO create function inside the actual swapchain render to the window


    //LOG(INFO) << "Called " << __func__;
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
