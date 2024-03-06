#pragma once
#include <string>

#include "openxr_includes.h"
#include "platform_manager.h"

// System
XrResult xrGeSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId);
XrResult xrGetSystemProperties(XrInstance instance, XrSystemId systemId, XrSystemProperties* properties);
XrResult xrEnumerateEnvironmentBlendModes(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput, uint32_t* environmentBlendModeCountOutput, XrEnvironmentBlendMode* environmentBlendModes);

// View configurations
XrResult xrEnumerateViewConfigurations(XrInstance instance, XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t* viewConfigurationTypeCountOutput, XrViewConfigurationType* viewConfigurationTypes);
XrResult xrGetViewConfigurationProperties(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties* configurationProperties);
XrResult xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views);

XrResult xrLocateViews(XrSession session, const XrViewLocateInfo* viewLocateInfo, XrViewState* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrView* views);

// Spaces
XrResult xrEnumerateReferenceSpaces(XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, XrReferenceSpaceType* spaces);

// Returns a newly create reference space or the space that already exists
XrResult xrCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo* createInfo, XrSpace* space);
XrResult xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df* bounds);
XrResult xrCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo* createInfo, XrSpace* space);
XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation* location);
XrResult xrDestroySpace(XrSpace space);

namespace  XRGameBridge {
    // System dummy values
    enum class GraphicsBackend {
        undefined = 0,
        D3D11 = 1,
        D3D12 = 2,
        Vulkan = 3,
        OpenGL = 4
    };

    struct GBVector2i
    {
        uint32_t x;
        uint32_t y;
    };

    enum class SRDisplay {
        SR_DISPLAY
    };

    struct GB_System {
        XrInstance instance;
        XrSystemId id;
        std::array<XrFormFactor, 2> supported_formfactors;
        XrFormFactor form_factor;
        SRDisplay sr_device;
        D3D_FEATURE_LEVEL feature_level;
        bool features_enumerated = false;
        GraphicsBackend active_graphics_backend;

        SR::Screen* sr_screen;
        SR::SwitchableLensHint* lens_hint;
    };

    // Spaces are basically transformation matrices.
    // They transform a point/orientation with respect to an XrSpace of the applications choosing
    struct GB_ReferenceSpace {
        XrSession session;
        XrSpace handle;
        XrReferenceSpaceType space_type;
        XrPosef pose_in_reference_space;
    };

    struct GB_ActionSpace {
        XrSession session;
        XrSpace handle;
        XrAction action;
        XrPath sub_action_path;
        XrPosef pose_in_action_space;
    };

    //GBVector2i GetDummyScreenResolution();

    //XrSystemProperties GetDummySystemProperties();

    GBVector2i GetSystemResolution(const GB_System& gb_system, XrFormFactor form_factor);
    GBVector2i GetNativeSystemResolution(const GB_System& gb_system);
    GBVector2i GetScaledSystemResolutionMainDisplay();
    XrSystemProperties GetSystemProperties(const GB_System& gb_system);
}
