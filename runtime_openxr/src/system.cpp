#include "system.h"

#include <array>

#include "easylogging++.h"
#include "openxr_includes.h"
#include "instance.h"
#include "session.h"

XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId) {
    //TODO The specification says basically that the system should be created before this call with a graphics api (getsystem not createsystem of course).
    //TODO Maybe during Instance creation or xrGetD3D12GraphicsRequirementsKHR
    static uint32_t system_creation_count = 1;

    XrFormFactor requested_formfactor;
    switch (getInfo->formFactor) {
    case XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY:
        requested_formfactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        break;
    case XR_FORM_FACTOR_HANDHELD_DISPLAY:
        requested_formfactor = XR_FORM_FACTOR_HANDHELD_DISPLAY;
        break;
    default:
        return XR_ERROR_FORM_FACTOR_UNSUPPORTED;
    }

    *systemId = system_creation_count;

    GameBridge::GB_System system;
    system.id = *systemId;
    system.instance = instance;
    system.requested_formfactor = requested_formfactor;
    system.sr_device = GameBridge::SRDisplay::SR_DISPLAY;

    GameBridge::systems.insert({*systemId, system});

    //system_creation_count++; // OpenXR supports only a single system?
    return XR_SUCCESS;
}

XrResult xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties* properties) {
    try {
        GameBridge::GB_System& system = GameBridge::systems.at(systemId);
    }
    catch (std::out_of_range& e) {
        return XR_ERROR_SYSTEM_INVALID;
    }
    catch (std::exception& e) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    *properties = GameBridge::get_dummy_system_properties();

    return XR_SUCCESS;
}

XrResult xrEnumerateEnvironmentBlendModes(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput, uint32_t* environmentBlendModeCountOutput, XrEnvironmentBlendMode* environmentBlendModes) {
    LOG(INFO) << "Requested view configuration type: " << viewConfigurationType;
    // SR only supports XR_ENVIRONMENT_BLEND_MODE_OPAQUE 
    const std::array supported_blend_modes = { XR_ENVIRONMENT_BLEND_MODE_OPAQUE };
    *environmentBlendModeCountOutput = supported_blend_modes.size();

    // Request for the extension array or the extension array itself
    if (environmentBlendModeCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    if (environmentBlendModeCapacityInput < 1) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }
    // Return whether the extension exists

    // Fill array
    memcpy_s(environmentBlendModes, environmentBlendModeCapacityInput * sizeof(XrEnvironmentBlendMode), supported_blend_modes.data(), supported_blend_modes.size() * sizeof(XrEnvironmentBlendMode));
    return XR_SUCCESS;
}

XrResult xrEnumerateViewConfigurations(XrInstance instance, XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t* viewConfigurationTypeCountOutput, XrViewConfigurationType* viewConfigurationTypes) {
    // TODO check if mono as primary is ok
    const std::array supported_view_configurations = { XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
    *viewConfigurationTypeCountOutput = supported_view_configurations.size();

    // Request for the extension array or the extension array itself
    if (viewConfigurationTypeCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    else if (viewConfigurationTypeCapacityInput < 1) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }
    // Return whether the extension exists
    else {
        // Fill array
        memcpy_s(viewConfigurationTypes, viewConfigurationTypeCapacityInput * sizeof(XrViewConfigurationType), supported_view_configurations.data(), supported_view_configurations.size() * sizeof(XrViewConfigurationType));
        return XR_SUCCESS;
    }
}

XrResult xrGetViewConfigurationProperties(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties* configurationProperties) {
    XrResult res = XR_ERROR_RUNTIME_FAILURE;

    switch (viewConfigurationType) {
    case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO:
        configurationProperties->viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO;
        configurationProperties->fovMutable = true; // TODO check if it's ok if the application changes the fov
        res = XR_SUCCESS;
        break;
    case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO:
        configurationProperties->viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        configurationProperties->fovMutable = true; // TODO check if it's ok if the application changes the fov
        res = XR_SUCCESS;
        break;
    default:;
        res = XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    return res;
}

XrResult xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views) {
    XrResult res = XR_ERROR_RUNTIME_FAILURE;

    auto screen_resolution = GameBridge::GetDummyScreenResolution();
    std::vector<XrViewConfigurationView> supported_views;

    if (viewConfigurationType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO || viewConfigurationType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) {
        XrViewConfigurationView view{};
        view.recommendedImageRectWidth = screen_resolution.x;
        view.maxImageRectWidth = screen_resolution.x;
        view.recommendedImageRectHeight = screen_resolution.y;
        view.maxImageRectHeight = screen_resolution.y;
        view.recommendedSwapchainSampleCount = 2; //TODO idk what this means
        view.maxSwapchainSampleCount = 2;

        supported_views.push_back(view);

        res = XR_SUCCESS;
    }
    else {
        res = XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    // Set output count
    *viewCountOutput = supported_views.size();

    // Request for the extension array or the extension array itself
    if (viewCapacityInput == 0) {
        res = XR_SUCCESS;
    }
    // Passed array not large enough
    else if (viewCapacityInput < supported_views.size()) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }
    else {
        memcpy_s(views, viewCapacityInput * sizeof(XrViewConfigurationView), supported_views.data(), supported_views.size() * sizeof(XrViewConfigurationView));
    }

    return res;
}

XrResult xrEnumerateReferenceSpaces(XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, XrReferenceSpaceType* spaces) {
    GameBridge::GB_Session* gb_session = reinterpret_cast<GameBridge::GB_Session*>(session);

    std::array reference_space_types{
        XR_REFERENCE_SPACE_TYPE_VIEW,
            XR_REFERENCE_SPACE_TYPE_LOCAL
    };

    *spaceCountOutput = reference_space_types.size();

    // Request for the extension array or the extension array itself
    if (spaceCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    if (spaceCapacityInput < reference_space_types.size()) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }

    memcpy_s(spaces, spaceCapacityInput * sizeof(XrReferenceSpaceType), reference_space_types.data(), reference_space_types.size() * sizeof(XrReferenceSpaceType));
    return XR_SUCCESS;
}

#include "openxr_functions.h"
XrResult xrCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo* createInfo, XrSpace* space) {
    static uint64_t reference_space_count = 1;
    XrSpace handle = reinterpret_cast<XrSpace>(reference_space_count);
    GameBridge::GB_ReferenceSpace new_space;
    new_space.session = session;
    new_space.handle = handle;
    new_space.pose_in_reference_space = createInfo->poseInReferenceSpace;

    if (createInfo->referenceSpaceType != XR_REFERENCE_SPACE_TYPE_VIEW  &&
        createInfo->referenceSpaceType != XR_REFERENCE_SPACE_TYPE_LOCAL &&
        createInfo->referenceSpaceType != XR_REFERENCE_SPACE_TYPE_STAGE) {
        return XR_ERROR_REFERENCE_SPACE_UNSUPPORTED;
    }

    const auto inserted = GameBridge::reference_spaces.insert({ handle, new_space });
    if (!inserted.second) {
        return XR_ERROR_RUNTIME_FAILURE;
    }

    reference_space_count++;
    *space = handle;

    return XR_SUCCESS;
}

XrResult xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df* bounds) {
    return test_return;
}

XrResult xrCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo* createInfo, XrSpace* space) {
    GameBridge::GB_ActionSpace new_space{};
    new_space.session = session;
    new_space.action = createInfo->action;
    new_space.sub_action_path = createInfo->subactionPath;
    new_space.pose_in_action_space = createInfo->poseInActionSpace;

    // Add action handle to sub action handle for a space handle hash
    XrSpace handle = reinterpret_cast<XrSpace>(reinterpret_cast<uint64_t>(new_space.action) + createInfo->subactionPath);
    *space = handle;
    GameBridge::action_spaces.insert({ handle, new_space });
    return XR_SUCCESS;
}

XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation* location) {
    return test_return;
}

XrResult xrDestroySpace(XrSpace space) {
    return test_return;
}
