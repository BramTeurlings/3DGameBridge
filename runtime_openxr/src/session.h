#pragma once

#include <string>
#include <vector>
#include "openxr_includes.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session);
XrResult xrDestroySession(XrSession session);

namespace XRGameBridge {
    struct GB_Session {
        XrSession id;
        XrInstance instance;
        XrSystemId system;
        //std::vector<XrActionSet> action_sets;

        // DirectX 12
        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<ID3D12CommandQueue> command_queue;
    };
}
