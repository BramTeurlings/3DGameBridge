#pragma once
#include "openxr_includes.h"
#include "platform_manager.h"
#include <unordered_map>

XR_DEFINE_HANDLE(XrSRContext)

namespace XRGameBridge
{
    SR::SRContext* CreateSrContext();

    inline std::unordered_map<XrSRContext, SR::Screen*> g_sr_screens;
    inline std::unordered_map<XrSRContext, SR::SwitchableLensHint*> g_sr_lenhints;
}
