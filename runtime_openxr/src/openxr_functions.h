#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "openxr_includes.h"

inline static XrResult test_return = XR_ERROR_RUNTIME_FAILURE;

inline XrResult xrPollEvent(XrInstance instance, XrEventDataBuffer* eventData) { return test_return; };
inline XrResult xrResultToString(XrInstance instance, XrResult value, char buffer[XR_MAX_RESULT_STRING_SIZE]) { return test_return; };
inline XrResult xrStructureTypeToString(XrInstance instance, XrStructureType value, char buffer[XR_MAX_STRUCTURE_NAME_SIZE]) { return test_return; };

inline XrResult xrEnumerateEnvironmentBlendModes(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t environmentBlendModeCapacityInput, uint32_t* environmentBlendModeCountOutput, XrEnvironmentBlendMode* environmentBlendModes) { return test_return; };

inline XrResult xrCreateSession(XrInstance instance, const XrSessionCreateInfo* createInfo, XrSession* session) { return test_return; };
inline XrResult xrDestroySession(XrSession session) { return test_return; };

inline XrResult xrEnumerateReferenceSpaces(XrSession session, uint32_t spaceCapacityInput, uint32_t* spaceCountOutput, XrReferenceSpaceType* spaces) { return test_return; };
inline XrResult xrCreateReferenceSpace(XrSession session, const XrReferenceSpaceCreateInfo* createInfo, XrSpace* space) { return test_return; };
inline XrResult xrGetReferenceSpaceBoundsRect(XrSession session, XrReferenceSpaceType referenceSpaceType, XrExtent2Df* bounds) { return test_return; };
inline XrResult xrCreateActionSpace(XrSession session, const XrActionSpaceCreateInfo* createInfo, XrSpace* space) { return test_return; };
inline XrResult xrLocateSpace(XrSpace space, XrSpace baseSpace, XrTime time, XrSpaceLocation* location) { return test_return; };
inline XrResult xrDestroySpace(XrSpace space) { return test_return; };

inline XrResult xrEnumerateViewConfigurations(XrInstance instance, XrSystemId systemId, uint32_t viewConfigurationTypeCapacityInput, uint32_t* viewConfigurationTypeCountOutput, XrViewConfigurationType* viewConfigurationTypes) { return test_return; };
inline XrResult xrGetViewConfigurationProperties(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, XrViewConfigurationProperties* configurationProperties) { return test_return; };
inline XrResult xrEnumerateViewConfigurationViews(XrInstance instance, XrSystemId systemId, XrViewConfigurationType viewConfigurationType, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrViewConfigurationView* views) { return test_return; };
inline XrResult xrEnumerateSwapchainFormats(XrSession session, uint32_t formatCapacityInput, uint32_t* formatCountOutput, int64_t* formats) { return test_return; };
inline XrResult xrCreateSwapchain(XrSession session, const XrSwapchainCreateInfo* createInfo, XrSwapchain* swapchain) { return test_return; };
inline XrResult xrDestroySwapchain(XrSwapchain swapchain) { return test_return; };
inline XrResult xrEnumerateSwapchainImages(XrSwapchain swapchain, uint32_t imageCapacityInput, uint32_t* imageCountOutput, XrSwapchainImageBaseHeader* images) { return test_return; };
inline XrResult xrAcquireSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageAcquireInfo* acquireInfo, uint32_t* index) { return test_return; };
inline XrResult xrWaitSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageWaitInfo* waitInfo) { return test_return; };
inline XrResult xrReleaseSwapchainImage(XrSwapchain swapchain, const XrSwapchainImageReleaseInfo* releaseInfo) { return test_return; };

inline XrResult xrBeginSession(XrSession session, const XrSessionBeginInfo* beginInfo) { return test_return; };
inline XrResult xrEndSession(XrSession session) { return test_return; };
inline XrResult xrRequestExitSession(XrSession session) { return test_return; };
inline XrResult xrWaitFrame(XrSession session, const XrFrameWaitInfo* frameWaitInfo, XrFrameState* frameState) { return test_return; };
inline XrResult xrBeginFrame(XrSession session, const XrFrameBeginInfo* frameBeginInfo) { return test_return; };
inline XrResult xrEndFrame(XrSession session, const XrFrameEndInfo* frameEndInfo) { return test_return; };

inline XrResult xrLocateViews(XrSession session, const XrViewLocateInfo* viewLocateInfo, XrViewState* viewState, uint32_t viewCapacityInput, uint32_t* viewCountOutput, XrView* views) { return test_return; };
inline XrResult xrStringToPath(XrInstance instance, const char* pathString, XrPath* path) { return test_return; };
inline XrResult xrPathToString(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer) { return test_return; };
inline XrResult xrCreateActionSet(XrInstance instance, const XrActionSetCreateInfo* createInfo, XrActionSet* actionSet) { return test_return; };
inline XrResult xrDestroyActionSet(XrActionSet actionSet) { return test_return; };
inline XrResult xrCreateAction(XrActionSet actionSet, const XrActionCreateInfo* createInfo, XrAction* action) { return test_return; };
inline XrResult xrDestroyAction(XrAction action) { return test_return; };
inline XrResult xrSuggestInteractionProfileBindings(XrInstance instance, const XrInteractionProfileSuggestedBinding* suggestedBindings) { return test_return; };
inline XrResult xrAttachSessionActionSets(XrSession session, const XrSessionActionSetsAttachInfo* attachInfo) { return test_return; };
inline XrResult xrGetCurrentInteractionProfile(XrSession session, XrPath topLevelUserPath, XrInteractionProfileState* interactionProfile) { return test_return; };
inline XrResult xrGetActionStateBoolean(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateBoolean* state) { return test_return; };
inline XrResult xrGetActionStateFloat(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateFloat* state) { return test_return; };
inline XrResult xrGetActionStateVector2f(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStateVector2f* state) { return test_return; };
inline XrResult xrGetActionStatePose(XrSession session, const XrActionStateGetInfo* getInfo, XrActionStatePose* state) { return test_return; };
inline XrResult xrSyncActions(XrSession session, const XrActionsSyncInfo* syncInfo) { return test_return; };
inline XrResult xrEnumerateBoundSourcesForAction(XrSession session, const XrBoundSourcesForActionEnumerateInfo* enumerateInfo, uint32_t sourceCapacityInput, uint32_t* sourceCountOutput, XrPath* sources) { return test_return; };
inline XrResult xrGetInputSourceLocalizedName(XrSession session, const XrInputSourceLocalizedNameGetInfo* getInfo, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer) { return test_return; };
inline XrResult xrApplyHapticFeedback(XrSession session, const XrHapticActionInfo* hapticActionInfo, const XrHapticBaseHeader* hapticFeedback) { return test_return; };
inline XrResult xrStopHapticFeedback(XrSession session, const XrHapticActionInfo* hapticActionInfo) { return test_return; };

//XrResult xrGetOpenGLGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLKHR* graphicsRequirements);
//XrResult xrGetVulkanInstanceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
//XrResult xrGetVulkanDeviceExtensionsKHR(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
//XrResult xrGetVulkanGraphicsDeviceKHR(XrInstance instance, XrSystemId systemId, VkInstance vkInstance, VkPhysicalDevice* vkPhysicalDevice);
//XrResult xrGetVulkanGraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements);
//XrResult xrGetVisibilityMaskKHR(XrSession session, XrViewConfigurationType viewConfigurationType, uint32_t viewIndex, XrVisibilityMaskTypeKHR visibilityMaskType, XrVisibilityMaskKHR* visibilityMask);
//XrResult xrConvertWin32PerformanceCounterToTimeKHR(XrInstance instance, const LARGE_INTEGER* performanceCounter, XrTime* time);
//XrResult xrConvertTimeToWin32PerformanceCounterKHR(XrInstance instance, XrTime time, LARGE_INTEGER* performanceCounter);
//XrResult xrCreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo, VkInstance* vulkanInstance, VkResult* vulkanResult);
//XrResult xrCreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo, VkDevice* vulkanDevice, VkResult* vulkanResult);
//XrResult xrGetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo, VkPhysicalDevice* vulkanPhysicalDevice);
//XrResult xrGetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements);
//XrResult xrCreateHandTrackerEXT(XrSession session, const XrHandTrackerCreateInfoEXT* createInfo, XrHandTrackerEXT* handTracker);
//XrResult xrDestroyHandTrackerEXT(XrHandTrackerEXT handTracker);
//XrResult xrLocateHandJointsEXT(XrHandTrackerEXT handTracker, const XrHandJointsLocateInfoEXT* locateInfo, XrHandJointLocationsEXT* locations);
//XrResult xrEnumerateDisplayRefreshRatesFB(XrSession session, uint32_t displayRefreshRateCapacityInput, uint32_t* displayRefreshRateCountOutput, float* displayRefreshRates);
//XrResult xrGetDisplayRefreshRateFB(XrSession session, float* displayRefreshRate);
//XrResult xrRequestDisplayRefreshRateFB(XrSession session, float displayRefreshRate);

namespace GameBridge {
    const std::unordered_map<std::string, PFN_xrVoidFunction> openxr_functions {
        { "rGetInstanceProcAddr", reinterpret_cast<PFN_xrVoidFunction>(xrGetInstanceProcAddr)},
        { "xrEnumerateInstanceExtensionProperties",     reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateInstanceExtensionProperties) },
        { "xrCreateInstance",                           reinterpret_cast<PFN_xrVoidFunction>(xrCreateInstance) },
        { "xrDestroyInstance",                          reinterpret_cast<PFN_xrVoidFunction>(xrDestroyInstance) },
        { "xrGetInstanceProperties",                    reinterpret_cast<PFN_xrVoidFunction>(xrGetInstanceProperties) },
        { "xrPollEvent",                                reinterpret_cast<PFN_xrVoidFunction>(xrPollEvent) },
        { "xrResultToString",                           reinterpret_cast<PFN_xrVoidFunction>(xrResultToString) },
        { "xrStructureTypeToString",                    reinterpret_cast<PFN_xrVoidFunction>(xrStructureTypeToString) },
        { "xrGetSystem",                                reinterpret_cast<PFN_xrVoidFunction>(xrGetSystem) },
        { "xrGetSystemProperties ",                     reinterpret_cast<PFN_xrVoidFunction>(xrGetSystemProperties) },
        { "xrEnumerateEnvironmentBlendModes",           reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateEnvironmentBlendModes) },
        { "xrCreateSession",                            reinterpret_cast<PFN_xrVoidFunction>(xrCreateSession) },
        { "xrDestroySession",                           reinterpret_cast<PFN_xrVoidFunction>(xrDestroySession) },
        { "xrEnumerateReferenceSpaces",                 reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateReferenceSpaces) },
        { "xrCreateReferenceSpace",                     reinterpret_cast<PFN_xrVoidFunction>(xrCreateReferenceSpace) },
        { "xrGetReferenceSpaceBoundsRect",              reinterpret_cast<PFN_xrVoidFunction>(xrGetReferenceSpaceBoundsRect) },
        { "xrCreateActionSpace",                        reinterpret_cast<PFN_xrVoidFunction>(xrCreateActionSpace) },
        { "xrLocateSpace",                              reinterpret_cast<PFN_xrVoidFunction>(xrLocateSpace) },
        { "xrDestroySpace",                             reinterpret_cast<PFN_xrVoidFunction>(xrDestroySpace) },
        { "xrEnumerateViewConfigurations",              reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateViewConfigurations) },
        { "xrGetViewConfigurationProperties",           reinterpret_cast<PFN_xrVoidFunction>(xrGetViewConfigurationProperties) },
        { "xrEnumerateViewConfigurationViews",          reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateViewConfigurationViews) },
        { "xrEnumerateSwapchainFormats",                reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateSwapchainFormats) },
        { "xrCreateSwapchain",                          reinterpret_cast<PFN_xrVoidFunction>(xrCreateSwapchain) },
        { "xrDestroySwapchain",                         reinterpret_cast<PFN_xrVoidFunction>(xrDestroySwapchain) },
        { "xrEnumerateSwapchainImages",                 reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateSwapchainImages) },
        { "xrAcquireSwapchainImage",                    reinterpret_cast<PFN_xrVoidFunction>(xrAcquireSwapchainImage) },
        { "xrWaitSwapchainImage",                       reinterpret_cast<PFN_xrVoidFunction>(xrWaitSwapchainImage) },
        { "xrReleaseSwapchainImage",                    reinterpret_cast<PFN_xrVoidFunction>(xrReleaseSwapchainImage) },
        { "xrBeginSession",                             reinterpret_cast<PFN_xrVoidFunction>(xrBeginSession) },
        { "xrEndSession",                               reinterpret_cast<PFN_xrVoidFunction>(xrEndSession) },
        { "xrRequestExitSession",                       reinterpret_cast<PFN_xrVoidFunction>(xrRequestExitSession) },
        { "xrWaitFrame",                                reinterpret_cast<PFN_xrVoidFunction>(xrWaitFrame) },
        { "xrBeginFrame",                               reinterpret_cast<PFN_xrVoidFunction>(xrBeginFrame) },
        { "xrEndFrame",                                 reinterpret_cast<PFN_xrVoidFunction>(xrEndFrame) },
        { "xrLocateViews",                              reinterpret_cast<PFN_xrVoidFunction>(xrLocateViews) },
        { "xrStringToPath",                             reinterpret_cast<PFN_xrVoidFunction>(xrStringToPath) },
        { "xrPathToString",                             reinterpret_cast<PFN_xrVoidFunction>(xrPathToString) },
        { "xrCreateActionSet",                          reinterpret_cast<PFN_xrVoidFunction>(xrCreateActionSet) },
        { "xrDestroyActionSet",                         reinterpret_cast<PFN_xrVoidFunction>(xrDestroyActionSet) },
        { "xrCreateAction",                             reinterpret_cast<PFN_xrVoidFunction>(xrCreateAction) },
        { "xrDestroyAction",                            reinterpret_cast<PFN_xrVoidFunction>(xrDestroyAction) },
        { "xrSuggestInteractionProfileBindings",        reinterpret_cast<PFN_xrVoidFunction>(xrSuggestInteractionProfileBindings) },
        { "xrAttachSessionActionSets",                  reinterpret_cast<PFN_xrVoidFunction>(xrAttachSessionActionSets) },
        { "xrGetCurrentInteractionProfile",             reinterpret_cast<PFN_xrVoidFunction>(xrGetCurrentInteractionProfile) },
        { "xrGetActionStateBoolean",                    reinterpret_cast<PFN_xrVoidFunction>(xrGetActionStateBoolean) },
        { "xrGetActionStateFloat",                      reinterpret_cast<PFN_xrVoidFunction>(xrGetActionStateFloat) },
        { "xrGetActionStateVector2f",                   reinterpret_cast<PFN_xrVoidFunction>(xrGetActionStateVector2f) },
        { "xrGetActionStatePose",                       reinterpret_cast<PFN_xrVoidFunction>(xrGetActionStatePose) },
        { "xrSyncActions",                              reinterpret_cast<PFN_xrVoidFunction>(xrSyncActions) },
        { "xrEnumerateBoundSourcesForAction",           reinterpret_cast<PFN_xrVoidFunction>(xrEnumerateBoundSourcesForAction) },
        { "xrGetInputSourceLocalizedName",              reinterpret_cast<PFN_xrVoidFunction>(xrGetInputSourceLocalizedName) },
        { "xrApplyHapticFeedback",                      reinterpret_cast<PFN_xrVoidFunction>(xrApplyHapticFeedback) },
        { "xrStopHapticFeedback",                       reinterpret_cast<PFN_xrVoidFunction>(xrStopHapticFeedback) },

        // Graphics extensions
        //{ "xrStopHapticFeedback",                       reinterpret_cast<PFN_xrVoidFunction>(xrGetD3D11GraphicsRequirementsKHR) },
        //{ "xrStopHapticFeedback",                       reinterpret_cast<PFN_xrVoidFunction>(xrGetD3D12GraphicsRequirementsKHR) }
    };

    const std::vector<XrExtensionProperties> supported_extensions{
        // Microsoft Windows extensions
        { XR_TYPE_EXTENSION_PROPERTIES, nullptr, XR_EXT_WIN32_APPCONTAINER_COMPATIBLE_EXTENSION_NAME, XR_EXT_win32_appcontainer_compatible_SPEC_VERSION },
        { XR_TYPE_EXTENSION_PROPERTIES, nullptr, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME, XR_KHR_win32_convert_performance_counter_time_SPEC_VERSION },
        { XR_TYPE_EXTENSION_PROPERTIES, nullptr, XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME, XR_KHR_composition_layer_depth_SPEC_VERSION },

        // Graphics Extensions
        { XR_TYPE_EXTENSION_PROPERTIES, nullptr, XR_KHR_D3D11_ENABLE_EXTENSION_NAME, XR_KHR_D3D11_enable_SPEC_VERSION },
        { XR_TYPE_EXTENSION_PROPERTIES, nullptr, XR_KHR_D3D12_ENABLE_EXTENSION_NAME, XR_KHR_D3D12_enable_SPEC_VERSION },
        //{ XR_TYPE_EXTENSION_PROPERTIES, nullptr, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME, XR_KHR_vulkan_enable2_SPEC_VERSION }
    };
}
