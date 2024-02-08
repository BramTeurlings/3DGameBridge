#include "session.h"

#include <stdexcept>

#include "easylogging++.h"
#include "openxr_functions.h"
#include "instance.h"
#include "system.h"
#include "settings.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session) {
    static uint64_t session_creation_count = 1;
    try {
        XRGameBridge::GB_System& system = XRGameBridge::systems.at(createInfo->systemId);
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
    XRGameBridge::GB_Session& new_session = XRGameBridge::sessions[handle];

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
    // Also action sets/actions attached to the session should be destroyed
    return test_return;
}

XrResult xrBeginSession(XrSession session, const XrSessionBeginInfo* beginInfo) {

    // TODO check if view configuration type is supported

    XRGameBridge::GB_Session& gb_session = XRGameBridge::sessions[session];
    if (gb_session.session_state == XR_SESSION_STATE_IDLE) {
        LOG(ERROR) << "Session not ready";
        return XR_ERROR_SESSION_NOT_READY;
    }
    if (gb_session.session_state != XR_SESSION_STATE_READY) {
        LOG(ERROR) << "Session is already running";
        return XR_ERROR_SESSION_RUNNING;
    }

    gb_session.view_configuration = beginInfo->primaryViewConfigurationType;

    return XR_SUCCESS;
}

XrResult xrEndSession(XrSession session) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrRequestExitSession(XrSession session) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
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
}
