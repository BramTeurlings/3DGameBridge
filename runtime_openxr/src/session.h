#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "openxr_includes.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session);
XrResult xrDestroySession(XrSession session);

XrResult xrBeginSession(XrSession session, const XrSessionBeginInfo* beginInfo);
XrResult xrEndSession(XrSession session);
XrResult xrRequestExitSession(XrSession session);

namespace XRGameBridge {
    struct GB_Session {
        XrSession id;
        XrInstance instance;
        XrSystemId system;
        XrSessionState session_state;
        XrViewConfigurationType view_configuration;
        //std::vector<XrActionSet> action_sets;

        //std
        std::chrono::high_resolution_clock::time_point session_epoch;

        // DirectX 12
        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<ID3D12CommandQueue> command_queue;
    };

    void ChangeSessionState(GB_Session& session, XrSessionState state);
}
