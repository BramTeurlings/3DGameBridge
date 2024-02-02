#pragma once

#include <string>
#include <unordered_map>

#include "openxr_includes.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session);
XrResult xrDestroySession(XrSession session);

namespace GameBridge {
    class GB_Session {
        XrSession id;
        std::vector<XrActionSet> action_sets;
    };
}
