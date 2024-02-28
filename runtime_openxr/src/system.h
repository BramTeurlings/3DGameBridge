#pragma once
#include <string>

#include "openxr_includes.h"
#include "compositor.h"

// System
XrResult xrGetSystem(XrInstance instance, const XrSystemGetInfo* getInfo, XrSystemId* systemId);
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

    inline GBVector2i GetDummyScreenResolution() {
        //TODO dependent on the SR screen, hopefully we can set reset this later on runtime. It would be cool to setup everything without having to connect to the sr service since that might take some time.
        // MS docs: The width/height of the client area for a full-screen window on the primary display monitor, in pixels.
        const uint32_t primary_display_res_x = static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN) / 2); // Divided by 2 since we render in sbs
        const uint32_t primary_display_res_y = static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN));
        return { primary_display_res_x, primary_display_res_y };
    }

    inline XrSystemProperties get_dummy_system_properties() {
        auto screen_resolution = XRGameBridge::GetDummyScreenResolution();

        XrSystemGraphicsProperties g_props{};
        g_props.maxLayerCount = 1;
        g_props.maxSwapchainImageWidth = screen_resolution.x;
        g_props.maxSwapchainImageHeight = screen_resolution.y;

        XrSystemTrackingProperties t_props{};
        t_props.positionTracking = false;
        t_props.orientationTracking = false;

        XrSystemProperties sys_props{
            XR_TYPE_SYSTEM_PROPERTIES,
            nullptr,
            1,
            0x354B, // USB Vendor ID
            "SR Monitor",
            g_props,
            t_props
        };
        return sys_props;
    }

    enum class SRDisplay {
        SR_DISPLAY
    };

    struct GB_System {
        XrInstance instance;
        XrSystemId id;
        XrFormFactor requested_formfactor;
        SRDisplay sr_device;
        D3D_FEATURE_LEVEL feature_level;
        bool features_enumerated = false;
        GraphicsBackend active_graphics_backend;
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
}
