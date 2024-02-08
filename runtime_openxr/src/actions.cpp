#include "actions.h"
#include "openxr_functions.h"

XrResult xrGetActionStateBoolean(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateBoolean* state) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrGetActionStateFloat(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateFloat* state) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrGetActionStateVector2f(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateVector2f* state) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrGetActionStatePose(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStatePose* state) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrSyncActions(XrSession session, const XrActionsSyncInfo* syncInfo) {
    LOG(INFO) << "Called";
    return XR_ERROR_RUNTIME_FAILURE;
}
