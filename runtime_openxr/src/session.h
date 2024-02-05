#pragma once

#include <string>
#include <vector>
#include "openxr_includes.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session);
XrResult xrDestroySession(XrSession session);

namespace GameBridge {
    struct GB_Session {
        XrSession id;
        XrInstance instance;
        XrSystemId system;
        //std::vector<XrActionSet> action_sets;
    };
}
