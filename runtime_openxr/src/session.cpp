#include "session.h"
#include "instance.h"

#include "openxr_functions.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session)
{
    GameBridge::GB_Instance* gb_instance = reinterpret_cast<GameBridge::GB_Instance*>(instance);


    if(!gb_instance->system.features_enumerated)
    {
        return XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING;
    }
    else if(gb_instance->system.id != createInfo->systemId)
    {
        return XR_ERROR_SYSTEM_INVALID;
    }

    *session = reinterpret_cast<XrSession>(&gb_instance->session);

    return XR_SUCCESS;
}

XrResult xrDestroySession(XrSession session)
{
    return test_return;
}
