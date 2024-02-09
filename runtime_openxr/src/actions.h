#pragma once
#include "openxr_includes.h"

XrResult xrSyncActions(XrSession session, const XrActionsSyncInfo* syncInfo);
XrResult xrGetActionStateBoolean(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateBoolean* state);
XrResult xrGetActionStateFloat(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateFloat* state);
XrResult xrGetActionStateVector2f(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateVector2f* state);
XrResult xrGetActionStatePose(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStatePose* state);

namespace XRGameBridge {

}
