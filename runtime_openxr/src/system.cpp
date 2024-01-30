#include "system.h"

#include <array>

#include "easylogging++.h"
#include "openxr_functions.h"
#include "instance.h"

XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId)
{
    GameBridge::GB_Instance* gb_instance = reinterpret_cast<GameBridge::GB_Instance*>(instance);
    XrFormFactor requested_formfactor;


    switch (getInfo->formFactor)
    {
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
    system->requested_formfactor = requested_formfactor;
    system->sr_device = GameBridge::SRDisplay::SR_DISPLAY;

    *systemId = reinterpret_cast<XrSystemId>(system);
    
    return XR_SUCCESS;
}

XrResult xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties* properties)
{
    return XR_ERROR_RUNTIME_FAILURE;
}

XrResult xrEnumerateEnvironmentBlendModes(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput, uint32_t* environmentBlendModeCountOutput, XrEnvironmentBlendMode* environmentBlendModes)
{
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

XrResult xrEnumerateViewConfigurations(XrInstance instance, XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t* viewConfigurationTypeCountOutput, XrViewConfigurationType* viewConfigurationTypes)
{
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

XrResult xrGetViewConfigurationProperties(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties* configurationProperties)
{
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
    default: ;
        res = XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;
    }

    return res;
}

XrResult xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId,XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views)
{
    const std::array supported_view_configurations = {
        XrViewConfigurationView{
        }
    };

    //TODO dependent on the SR screen, hopefully we can set reset this later on runtime. It would be cool to setup everything without having to connect to the sr service since that might take some time.
    // MS docs: The width/height of the client area for a full-screen window on the primary display monitor, in pixels. 
    const uint32_t primary_display_res_x = static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN));
    const uint32_t primary_display_res_y = static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN));

    views->recommendedImageRectWidth = primary_display_res_x;
    views->maxImageRectWidth = primary_display_res_x;
    views->recommendedImageRectHeight = primary_display_res_y;
    views->maxImageRectHeight = primary_display_res_y;
    views->recommendedSwapchainSampleCount = 2; //TODO idk what this means
    views->maxSwapchainSampleCount = 2;

    return test_return;
}
