#include "system.h"

#include <array>

#include "easylogging++.h"
#include "openxr_includes.h"
#include "instance.h"
#include "system.h"
#include "session.h"

XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId) {
    // TODO need a better and safer way to reference instances
    GameBridge::GB_Instance* gb_instance = reinterpret_cast<GameBridge::GB_Instance*>(instance);
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

    GameBridge::GB_System* system = &gb_instance->system;
    *systemId = reinterpret_cast<XrSystemId>(system);

    system->id = *systemId;
    system->requested_formfactor = requested_formfactor;
    system->sr_device = GameBridge::SRDisplay::SR_DISPLAY;

    return XR_SUCCESS;
}

XrResult xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties* properties) {
    return XR_ERROR_RUNTIME_FAILURE;
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
    else if (environmentBlendModeCapacityInput < 1) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }
    // Return whether the extension exists
    else {
        // Fill array
        memcpy_s(environmentBlendModes, environmentBlendModeCapacityInput * sizeof(XrEnvironmentBlendMode), supported_blend_modes.data(), supported_blend_modes.size() * sizeof(XrEnvironmentBlendMode));
        return XR_SUCCESS;
    }
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

    //TODO dependent on the SR screen, hopefully we can set reset this later on runtime. It would be cool to setup everything without having to connect to the sr service since that might take some time.
    // MS docs: The width/height of the client area for a full-screen window on the primary display monitor, in pixels. 
    const uint32_t primary_display_res_x = static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN));
    const uint32_t primary_display_res_y = static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN));

    std::vector<XrViewConfigurationView> supported_views;

    if (viewConfigurationType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO || viewConfigurationType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) {
        XrViewConfigurationView view{};
        view.recommendedImageRectWidth = primary_display_res_x;
        view.maxImageRectWidth = primary_display_res_x;
        view.recommendedImageRectHeight = primary_display_res_y;
        view.maxImageRectHeight = primary_display_res_y;
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
    createInfo->poseInReferenceSpace;
    createInfo->referenceSpaceType;

    GameBridge::GB_Session* gb_session = reinterpret_cast<GameBridge::GB_Session*>(session);
    return test_return;
}

XrResult xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df* bounds) {
    return test_return;
}

XrResult xrCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo* createInfo, XrSpace* space) {
    return test_return;
}

XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation* location) {
    return test_return;
}

XrResult xrDestroySpace(XrSpace space) {
    return test_return;
}
