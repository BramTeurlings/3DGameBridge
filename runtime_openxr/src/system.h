#pragma once

#include "openxr_includes.h"

// System
XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId);
XrResult xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties* properties);
XrResult xrEnumerateEnvironmentBlendModes(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput, uint32_t* environmentBlendModeCountOutput, XrEnvironmentBlendMode* environmentBlendModes);

// View configurations
XrResult xrEnumerateViewConfigurations(XrInstance instance, XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t* viewConfigurationTypeCountOutput, XrViewConfigurationType* viewConfigurationTypes);
XrResult xrGetViewConfigurationProperties(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties* configurationProperties);
XrResult xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views);

// Spaces
XrResult xrEnumerateReferenceSpaces(XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, XrReferenceSpaceType* spaces);
XrResult xrCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo* createInfo, XrSpace* space);
XrResult xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df* bounds);
XrResult xrCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo* createInfo, XrSpace* space);
XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation* location);
XrResult xrDestroySpace(XrSpace space);

namespace  GameBridge {
    enum class SRDisplay {
        SR_DISPLAY
    };

    class GB_System {
    public:
        XrSystemId id;
        XrFormFactor requested_formfactor;
        SRDisplay sr_device;
        D3D_FEATURE_LEVEL feature_level;
        bool features_enumerated = false;
    };

    struct GB_Space {
        XrSession session;
        XrSpace handle;
        XrAction action;
        XrPath sub_action_path;
        XrPosef pose_in_action_space;
    };
}