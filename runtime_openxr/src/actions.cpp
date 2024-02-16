#include "actions.h"

#include "instance.h"
#include "openxr_functions.h"

XrResult xrSyncActions(XrSession session, const XrActionsSyncInfo* syncInfo) {
    const std::vector<XrActiveActionSet> active_action_sets(syncInfo->activeActionSets, syncInfo->activeActionSets + syncInfo->countActiveActionSets);

    try {
        for (auto& set : active_action_sets) {
            auto& gb_action_set = XRGameBridge::g_action_sets.at(set.actionSet);
            // Check if it is attached to the passed session parameter
            if (gb_action_set.session != session) {
                return XR_ERROR_ACTIONSET_NOT_ATTACHED;
            }
        }
    }
    catch (std::out_of_range& e) {
        return XR_ERROR_HANDLE_INVALID;
    }
    catch (std::exception& e) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}

XrResult xrGetActionStateBoolean(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateBoolean* state) {
    state->isActive = false;
    state->currentState = false;
    state->changedSinceLastSync = false;
    state->lastChangeTime = 0;
    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}

XrResult xrGetActionStateFloat(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateFloat* state) {

    XRGameBridge::GB_Session& gb_session = XRGameBridge::g_sessions[session];

    state->isActive = false;
    state->currentState = 0.f;
    state->changedSinceLastSync = false;
    state->lastChangeTime = 0;

    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}

XrResult xrGetActionStateVector2f(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateVector2f* state) {

    state->isActive = false;
    state->currentState = {0.f};
    state->changedSinceLastSync = false;
    state->lastChangeTime = 0;

    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}

XrResult xrGetActionStatePose(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStatePose* state) {
    state->isActive = false;

    //LOG(INFO) << "Called " << __func__;
    return XR_SUCCESS;
}
