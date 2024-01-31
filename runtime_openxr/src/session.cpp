#include "session.h"
#include "instance.h"

#include "openxr_functions.h"

XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session)
{
    GameBridge::GB_Instance* gb_instance = reinterpret_cast<GameBridge::GB_Instance*>(instance);

    return test_return;
}

XrResult xrDestroySession(XrSession session)
{
    return test_return;
}
