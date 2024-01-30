#include "Bindings.h"

#include "Instance.h"
#include "Session.h"
#include "System.h"
#include "Space.h"
#include "Swapchain.h"
#include "ActionSet.h"
#include "Extensions/DebugUtils.h"
#include <openxr/openxr_reflection.h>

DEFINE_LOG_CATEGORY(FLogOpenXRInterface, "OpenXR");

using namespace WyvernEngine;

#define FunctionStub(Name) XrResult Name() { Log(FLogOpenXRInterface, Error, "Trying to call stubbed function: " #Name); return XR_ERROR_RUNTIME_FAILURE; }

// Stubbed functions
#define EnumStubs(Macro)\
Macro(xrSetInputDeviceActiveEXT)\
Macro(xrSetInputDeviceStateBoolEXT)\
Macro(xrSetInputDeviceStateFloatEXT)\
Macro(xrSetInputDeviceStateVector2fEXT)\
Macro(xrSetInputDeviceLocationEXT)\
Macro(xrCreateSpatialGraphNodeSpaceMSFT)\
Macro(xrTryCreateSpatialGraphStaticNodeBindingMSFT)\
Macro(xrDestroySpatialGraphNodeBindingMSFT)\
Macro(xrGetSpatialGraphNodeBindingPropertiesMSFT)\
Macro(xrCreateHandTrackerEXT)\
Macro(xrDestroyHandTrackerEXT)\
Macro(xrLocateHandJointsEXT)\
Macro(xrCreateHandMeshSpaceMSFT)\
Macro(xrUpdateHandMeshMSFT)\
Macro(xrGetControllerModelKeyMSFT)\
Macro(xrLoadControllerModelMSFT)\
Macro(xrGetControllerModelPropertiesMSFT)\
Macro(xrGetControllerModelStateMSFT)\
Macro(xrCreateSpatialAnchorFromPerceptionAnchorMSFT)\
Macro(xrTryGetPerceptionAnchorFromSpatialAnchorMSFT)\
Macro(xrEnumerateReprojectionModesMSFT)\
Macro(xrUpdateSwapchainFB)\
Macro(xrGetSwapchainStateFB)\
Macro(xrEnumerateSceneComputeFeaturesMSFT)\
Macro(xrCreateSceneObserverMSFT)\
Macro(xrDestroySceneObserverMSFT)\
Macro(xrCreateSceneMSFT)\
Macro(xrDestroySceneMSFT)\
Macro(xrComputeNewSceneMSFT)\
Macro(xrGetSceneComputeStateMSFT)\
Macro(xrGetSceneComponentsMSFT)\
Macro(xrLocateSceneComponentsMSFT)\
Macro(xrGetSceneMeshBuffersMSFT)\
Macro(xrDeserializeSceneMSFT)\
Macro(xrGetSerializedSceneFragmentDataMSFT)\
Macro(xrEnumerateDisplayRefreshRatesFB)\
Macro(xrGetDisplayRefreshRateFB)\
Macro(xrRequestDisplayRefreshRateFB)\
Macro(xrEnumerateViveTrackerPathsHTCX)\
Macro(xrCreateFacialTrackerHTC)\
Macro(xrDestroyFacialTrackerHTC)\
Macro(xrGetFacialExpressionsHTC)\
Macro(xrEnumerateColorSpacesFB)\
Macro(xrSetColorSpaceFB)\
Macro(xrGetHandMeshFB)\
Macro(xrCreateSpatialAnchorFB)\
Macro(xrGetSpaceUuidFB)\
Macro(xrEnumerateSpaceSupportedComponentsFB)\
Macro(xrSetSpaceComponentStatusFB)\
Macro(xrGetSpaceComponentStatusFB)\
Macro(xrCreateFoveationProfileFB)\
Macro(xrDestroyFoveationProfileFB)\
Macro(xrQuerySystemTrackedKeyboardFB)\
Macro(xrCreateKeyboardSpaceFB)\
Macro(xrCreateTriangleMeshFB)\
Macro(xrDestroyTriangleMeshFB)\
Macro(xrTriangleMeshGetVertexBufferFB)\
Macro(xrTriangleMeshGetIndexBufferFB)\
Macro(xrTriangleMeshBeginUpdateFB)\
Macro(xrTriangleMeshEndUpdateFB)\
Macro(xrTriangleMeshBeginVertexBufferUpdateFB)\
Macro(xrTriangleMeshEndVertexBufferUpdateFB)\
Macro(xrCreatePassthroughFB)\
Macro(xrDestroyPassthroughFB)\
Macro(xrPassthroughStartFB)\
Macro(xrPassthroughPauseFB)\
Macro(xrCreatePassthroughLayerFB)\
Macro(xrDestroyPassthroughLayerFB)\
Macro(xrPassthroughLayerPauseFB)\
Macro(xrPassthroughLayerResumeFB)\
Macro(xrPassthroughLayerSetStyleFB)\
Macro(xrCreateGeometryInstanceFB)\
Macro(xrDestroyGeometryInstanceFB)\
Macro(xrGeometryInstanceSetTransformFB)\
Macro(xrEnumerateRenderModelPathsFB)\
Macro(xrGetRenderModelPropertiesFB)\
Macro(xrLoadRenderModelFB)\
Macro(xrSetEnvironmentDepthEstimationVARJO)\
Macro(xrSetMarkerTrackingVARJO)\
Macro(xrSetMarkerTrackingTimeoutVARJO)\
Macro(xrSetMarkerTrackingPredictionVARJO)\
Macro(xrGetMarkerSizeVARJO)\
Macro(xrCreateMarkerSpaceVARJO)\
Macro(xrSetViewOffsetVARJO)\
Macro(xrCreateSpatialAnchorStoreConnectionMSFT)\
Macro(xrDestroySpatialAnchorStoreConnectionMSFT)\
Macro(xrPersistSpatialAnchorMSFT)\
Macro(xrEnumeratePersistedSpatialAnchorNamesMSFT)\
Macro(xrCreateSpatialAnchorFromPersistedNameMSFT)\
Macro(xrUnpersistSpatialAnchorMSFT)\
Macro(xrClearSpatialAnchorStoreMSFT)\
Macro(xrQuerySpacesFB)\
Macro(xrRetrieveSpaceQueryResultsFB)\
Macro(xrSaveSpaceFB)\
Macro(xrEraseSpaceFB)\
Macro(xrGetAudioOutputDeviceGuidOculus)\
Macro(xrGetAudioInputDeviceGuidOculus)\
Macro(xrGetSpaceBoundingBox2DFB)\
Macro(xrGetSpaceBoundingBox3DFB)\
Macro(xrGetSpaceSemanticLabelsFB)\
Macro(xrGetSpaceBoundary2DFB)\
Macro(xrGetSpaceRoomLayoutFB)\
Macro(xrSetDigitalLensControlALMALENCE)\
Macro(xrGetSpaceContainerFB)\
Macro(xrPassthroughLayerSetKeyboardHandsIntensityFB)\
Macro(xrEnumeratePerformanceMetricsCounterPathsMETA)\
Macro(xrSetPerformanceMetricsStateMETA)\
Macro(xrGetPerformanceMetricsStateMETA)\
Macro(xrQueryPerformanceMetricsCounterMETA)\
Macro(xrGetVisibilityMaskKHR)\
Macro(xrConvertTimespecTimeToTimeKHR)\
Macro(xrConvertTimeToTimespecTimeKHR)\
Macro(xrPerfSettingsSetPerformanceLevelEXT)\
Macro(xrThermalGetTemperatureTrendEXT)\
Macro(xrSessionBeginDebugUtilsLabelRegionEXT)\
Macro(xrSessionEndDebugUtilsLabelRegionEXT)\
Macro(xrSessionInsertDebugUtilsLabelEXT)\
Macro(xrCreateSpatialAnchorMSFT)\
Macro(xrCreateSpatialAnchorSpaceMSFT)\
Macro(xrDestroySpatialAnchorMSFT)\
Macro(xrSetAndroidApplicationThreadKHR)\
Macro(xrCreateSwapchainAndroidSurfaceKHR)\
Macro(xrGetOpenGLESGraphicsRequirementsKHR)\
Macro(xrCreateBodyTrackerFB)\
Macro(xrDestroyBodyTrackerFB)\
Macro(xrLocateBodyJointsFB)\
Macro(xrGetBodySkeletonFB)\
Macro(xrShareSpacesFB)\
Macro(xrRequestSceneCaptureFB)\
Macro(xrGetFoveationEyeTrackedStateMETA)\
Macro(xrCreateFaceTrackerFB)\
Macro(xrDestroyFaceTrackerFB)\
Macro(xrGetFaceExpressionWeightsFB)\
Macro(xrCreateEyeTrackerFB)\
Macro(xrDestroyEyeTrackerFB)\
Macro(xrGetEyeGazesFB)\
Macro(xrGetDeviceSampleRateFB)\
Macro(xrEnumerateExternalCamerasOCULUS)\
Macro(xrSaveSpaceListFB)\
Macro(xrCreateSpaceUserFB)\
Macro(xrGetSpaceUserIdFB)\
Macro(xrDestroySpaceUserFB)\
Macro(xrSetTrackingOptimizationSettingsHintQCOM)\
Macro(xrCreatePassthroughHTC)\
Macro(xrDestroyPassthroughHTC)\
Macro(xrApplyFoveationHTC)\
Macro(xrApplyForceFeedbackCurlMNDX)\
Macro(xrGetPassthroughPreferencesMETA)\
Macro(xrCreateVirtualKeyboardMETA)\
Macro(xrDestroyVirtualKeyboardMETA)\
Macro(xrCreateVirtualKeyboardSpaceMETA)\
Macro(xrSuggestVirtualKeyboardLocationMETA)\
Macro(xrGetVirtualKeyboardScaleMETA)\
Macro(xrSetVirtualKeyboardModelVisibilityMETA)\
Macro(xrGetVirtualKeyboardModelAnimationStatesMETA)\
Macro(xrGetVirtualKeyboardDirtyTexturesMETA)\
Macro(xrGetVirtualKeyboardTextureDataMETA)\
Macro(xrSendVirtualKeyboardInputMETA)\
Macro(xrChangeVirtualKeyboardTextContextMETA)\
Macro(xrCreatePassthroughColorLutMETA)\
Macro(xrDestroyPassthroughColorLutMETA)\
Macro(xrUpdatePassthroughColorLutMETA)\
Macro(xrCreatePlaneDetectorEXT)\
Macro(xrDestroyPlaneDetectorEXT)\
Macro(xrBeginPlaneDetectionEXT)\
Macro(xrGetPlaneDetectionStateEXT)\
Macro(xrGetPlaneDetectionsEXT)\
Macro(xrGetPlanePolygonBufferEXT)\
Macro(xrCreateSpaceFromCoordinateFrameUIDML)

EnumStubs(FunctionStub)

#define ListStub(Name) #Name ,
FArray<FString> StubbedFunctions = 
{
    EnumStubs(ListStub)
};

#define LinkFunctionExt(Name, Extension) std::pair<std::string, FXRFunctionInfo>(#Name, {(PFN_xrVoidFunction)&(Name), Extension}),
#define LinkFunctionExtName(Name, Extension) LinkFunctionExt(Name, #Extension)
#define LinkFunction(Name) LinkFunctionExt(Name, nullptr)

FMap<FString, FXRFunctionInfo> FunctionMapping = std::map<FString, FXRFunctionInfo>
{
    LinkFunction(xrEnumerateInstanceExtensionProperties)
    LinkFunction(xrCreateInstance)
    LinkFunction(xrDestroyInstance)
    LinkFunction(xrGetSystem)
    LinkFunction(xrEnumerateEnvironmentBlendModes)
    LinkFunction(xrCreateSession)
    LinkFunction(xrDestroySession)
    LinkFunction(xrCreateReferenceSpace)
    LinkFunction(xrEnumerateViewConfigurationViews)
    LinkFunction(xrCreateSwapchain)
    LinkFunction(xrEnumerateSwapchainImages)
    LinkFunction(xrPollEvent)
    LinkFunction(xrBeginSession)
    LinkFunction(xrWaitFrame)
    LinkFunction(xrBeginFrame)
    LinkFunction(xrLocateViews)
    LinkFunction(xrAcquireSwapchainImage)
    LinkFunction(xrReleaseSwapchainImage)
    LinkFunction(xrWaitSwapchainImage)
    LinkFunction(xrEndFrame)
    LinkFunction(xrDestroySpace)
    LinkFunction(xrEnumerateViewConfigurations)
    LinkFunction(xrGetViewConfigurationProperties)
    LinkFunction(xrEnumerateSwapchainFormats)
    LinkFunction(xrDestroySwapchain)
    LinkFunction(xrEndSession)
    LinkFunction(xrRequestExitSession)
    LinkFunction(xrStringToPath)
    LinkFunction(xrPathToString)
    LinkFunction(xrCreateActionSet)
    LinkFunction(xrDestroyActionSet)
    LinkFunction(xrCreateAction)
    LinkFunction(xrDestroyAction)
    LinkFunction(xrSuggestInteractionProfileBindings)
    LinkFunction(xrAttachSessionActionSets)
    LinkFunction(xrGetCurrentInteractionProfile)
    LinkFunction(xrGetActionStateBoolean)
    LinkFunction(xrGetActionStateFloat)
    LinkFunction(xrGetActionStateVector2f)
    LinkFunction(xrGetActionStatePose)
    LinkFunction(xrSyncActions)
    LinkFunction(xrEnumerateBoundSourcesForAction)
    LinkFunction(xrGetInputSourceLocalizedName)
    LinkFunction(xrApplyHapticFeedback)
    LinkFunction(xrStopHapticFeedback)
    LinkFunction(xrGetInstanceProperties)
    LinkFunction(xrResultToString)
    LinkFunction(xrStructureTypeToString)
    LinkFunction(xrGetSystemProperties)
    LinkFunction(xrEnumerateReferenceSpaces)
    LinkFunction(xrGetReferenceSpaceBoundsRect)
    LinkFunction(xrCreateActionSpace)
    LinkFunction(xrLocateSpace)

    LinkFunctionExt(xrSetInputDeviceActiveEXT, XR_EXT_CONFORMANCE_AUTOMATION_EXTENSION_NAME)
    LinkFunctionExt(xrSetInputDeviceStateBoolEXT, XR_EXT_CONFORMANCE_AUTOMATION_EXTENSION_NAME)
    LinkFunctionExt(xrSetInputDeviceStateFloatEXT, XR_EXT_CONFORMANCE_AUTOMATION_EXTENSION_NAME)
    LinkFunctionExt(xrSetInputDeviceStateVector2fEXT, XR_EXT_CONFORMANCE_AUTOMATION_EXTENSION_NAME)
    LinkFunctionExt(xrSetInputDeviceLocationEXT, XR_EXT_CONFORMANCE_AUTOMATION_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialGraphNodeSpaceMSFT, XR_MSFT_SPATIAL_GRAPH_BRIDGE_EXTENSION_NAME)
    LinkFunctionExt(xrTryCreateSpatialGraphStaticNodeBindingMSFT, XR_MSFT_SPATIAL_GRAPH_BRIDGE_EXTENSION_NAME)
    LinkFunctionExt(xrDestroySpatialGraphNodeBindingMSFT, XR_MSFT_SPATIAL_GRAPH_BRIDGE_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpatialGraphNodeBindingPropertiesMSFT, XR_MSFT_SPATIAL_GRAPH_BRIDGE_EXTENSION_NAME)
    LinkFunctionExt(xrCreateHandTrackerEXT, XR_EXT_HAND_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyHandTrackerEXT, XR_EXT_HAND_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrLocateHandJointsEXT, XR_EXT_HAND_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrCreateHandMeshSpaceMSFT, XR_MSFT_HAND_TRACKING_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrUpdateHandMeshMSFT, XR_MSFT_HAND_TRACKING_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrGetControllerModelKeyMSFT, XR_MSFT_CONTROLLER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrLoadControllerModelMSFT, XR_MSFT_CONTROLLER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrGetControllerModelPropertiesMSFT, XR_MSFT_CONTROLLER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrGetControllerModelStateMSFT, XR_MSFT_CONTROLLER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialAnchorFromPerceptionAnchorMSFT, XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME)
    LinkFunctionExt(xrTryGetPerceptionAnchorFromSpatialAnchorMSFT, XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateReprojectionModesMSFT, XR_MSFT_COMPOSITION_LAYER_REPROJECTION_EXTENSION_NAME)
    LinkFunctionExt(xrUpdateSwapchainFB, XR_FB_SWAPCHAIN_UPDATE_STATE_EXTENSION_NAME)
    LinkFunctionExt(xrGetSwapchainStateFB, XR_FB_SWAPCHAIN_UPDATE_STATE_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateSceneComputeFeaturesMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSceneObserverMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrDestroySceneObserverMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSceneMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrDestroySceneMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrComputeNewSceneMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrGetSceneComputeStateMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrGetSceneComponentsMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrLocateSceneComponentsMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrGetSceneMeshBuffersMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrDeserializeSceneMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrGetSerializedSceneFragmentDataMSFT, XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateDisplayRefreshRatesFB, XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME)
    LinkFunctionExt(xrGetDisplayRefreshRateFB, XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME)
    LinkFunctionExt(xrRequestDisplayRefreshRateFB, XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateViveTrackerPathsHTCX, XR_HTCX_VIVE_TRACKER_INTERACTION_EXTENSION_NAME)
    LinkFunctionExt(xrCreateFacialTrackerHTC, XR_HTC_FACIAL_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyFacialTrackerHTC, XR_HTC_FACIAL_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrGetFacialExpressionsHTC, XR_HTC_FACIAL_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateColorSpacesFB, XR_FB_COLOR_SPACE_EXTENSION_NAME)
    LinkFunctionExt(xrSetColorSpaceFB, XR_FB_COLOR_SPACE_EXTENSION_NAME)
    LinkFunctionExt(xrGetHandMeshFB, XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialAnchorFB, XR_FB_SPATIAL_ENTITY_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceUuidFB, XR_FB_SPATIAL_ENTITY_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateSpaceSupportedComponentsFB, XR_FB_SPATIAL_ENTITY_EXTENSION_NAME)
    LinkFunctionExt(xrSetSpaceComponentStatusFB, XR_FB_SPATIAL_ENTITY_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceComponentStatusFB, XR_FB_SPATIAL_ENTITY_EXTENSION_NAME)
    LinkFunctionExt(xrCreateFoveationProfileFB, XR_FB_FOVEATION_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyFoveationProfileFB, XR_FB_FOVEATION_EXTENSION_NAME)
    LinkFunctionExt(xrQuerySystemTrackedKeyboardFB, XR_FB_KEYBOARD_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrCreateKeyboardSpaceFB, XR_FB_KEYBOARD_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrCreateTriangleMeshFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyTriangleMeshFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrTriangleMeshGetVertexBufferFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrTriangleMeshGetIndexBufferFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrTriangleMeshBeginUpdateFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrTriangleMeshEndUpdateFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrTriangleMeshBeginVertexBufferUpdateFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrTriangleMeshEndVertexBufferUpdateFB, XR_FB_TRIANGLE_MESH_EXTENSION_NAME)
    LinkFunctionExt(xrCreatePassthroughFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyPassthroughFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrPassthroughStartFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrPassthroughPauseFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrCreatePassthroughLayerFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyPassthroughLayerFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrPassthroughLayerPauseFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrPassthroughLayerResumeFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrPassthroughLayerSetStyleFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrCreateGeometryInstanceFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyGeometryInstanceFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrGeometryInstanceSetTransformFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrEnumerateRenderModelPathsFB, XR_FB_RENDER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrGetRenderModelPropertiesFB, XR_FB_RENDER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrLoadRenderModelFB, XR_FB_RENDER_MODEL_EXTENSION_NAME)
    LinkFunctionExt(xrSetEnvironmentDepthEstimationVARJO, XR_VARJO_ENVIRONMENT_DEPTH_ESTIMATION_EXTENSION_NAME)
    LinkFunctionExt(xrSetMarkerTrackingVARJO, XR_VARJO_MARKER_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrSetMarkerTrackingTimeoutVARJO, XR_VARJO_MARKER_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrSetMarkerTrackingPredictionVARJO, XR_VARJO_MARKER_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrGetMarkerSizeVARJO, XR_VARJO_MARKER_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrCreateMarkerSpaceVARJO, XR_VARJO_MARKER_TRACKING_EXTENSION_NAME)
    LinkFunctionExt(xrSetViewOffsetVARJO, XR_VARJO_VIEW_OFFSET_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialAnchorStoreConnectionMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrDestroySpatialAnchorStoreConnectionMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrPersistSpatialAnchorMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrEnumeratePersistedSpatialAnchorNamesMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialAnchorFromPersistedNameMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrUnpersistSpatialAnchorMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrClearSpatialAnchorStoreMSFT, XR_MSFT_SPATIAL_ANCHOR_PERSISTENCE_EXTENSION_NAME)
    LinkFunctionExt(xrQuerySpacesFB, XR_FB_SPATIAL_ENTITY_STORAGE_EXTENSION_NAME)
    LinkFunctionExt(xrRetrieveSpaceQueryResultsFB, XR_FB_SPATIAL_ENTITY_STORAGE_EXTENSION_NAME)
    LinkFunctionExt(xrSaveSpaceFB, XR_FB_SPATIAL_ENTITY_STORAGE_EXTENSION_NAME)
    LinkFunctionExt(xrEraseSpaceFB, XR_FB_SPATIAL_ENTITY_STORAGE_EXTENSION_NAME)
    LinkFunctionExt(xrGetAudioOutputDeviceGuidOculus, XR_OCULUS_AUDIO_DEVICE_GUID_EXTENSION_NAME)
    LinkFunctionExt(xrGetAudioInputDeviceGuidOculus, XR_OCULUS_AUDIO_DEVICE_GUID_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceBoundingBox2DFB, XR_FB_SCENE_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceBoundingBox3DFB, XR_FB_SCENE_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceSemanticLabelsFB, XR_FB_SCENE_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceBoundary2DFB, XR_FB_SCENE_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceRoomLayoutFB, XR_FB_SCENE_EXTENSION_NAME)
    LinkFunctionExt(xrSetDigitalLensControlALMALENCE, XR_ALMALENCE_DIGITAL_LENS_CONTROL_EXTENSION_NAME)
    LinkFunctionExt(xrGetSpaceContainerFB, XR_FB_SPATIAL_ENTITY_CONTAINER_EXTENSION_NAME)
    LinkFunctionExt(xrPassthroughLayerSetKeyboardHandsIntensityFB, XR_FB_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrEnumeratePerformanceMetricsCounterPathsMETA, XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME)
    LinkFunctionExt(xrSetPerformanceMetricsStateMETA, XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME)
    LinkFunctionExt(xrGetPerformanceMetricsStateMETA, XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME)
    LinkFunctionExt(xrQueryPerformanceMetricsCounterMETA, XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME)
    LinkFunctionExt(xrGetOpenGLGraphicsRequirementsKHR, XR_KHR_OPENGL_ENABLE_EXTENSION_NAME)
    LinkFunctionExt(xrGetD3D11GraphicsRequirementsKHR, XR_KHR_D3D11_ENABLE_EXTENSION_NAME)
    LinkFunctionExt(xrGetD3D12GraphicsRequirementsKHR, XR_KHR_D3D12_ENABLE_EXTENSION_NAME)
    LinkFunctionExt(xrGetVisibilityMaskKHR, XR_KHR_VISIBILITY_MASK_EXTENSION_NAME)
    LinkFunctionExt(xrConvertWin32PerformanceCounterToTimeKHR, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME)
    LinkFunctionExt(xrConvertTimeToWin32PerformanceCounterKHR, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME)
    LinkFunctionExtName(xrConvertTimespecTimeToTimeKHR, XR_KHR_convert_timespec_time)
    LinkFunctionExtName(xrConvertTimeToTimespecTimeKHR, XR_KHR_convert_timespec_time)
    LinkFunctionExt(xrPerfSettingsSetPerformanceLevelEXT, XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME)
    LinkFunctionExt(xrThermalGetTemperatureTrendEXT, XR_EXT_THERMAL_QUERY_EXTENSION_NAME)
    LinkFunctionExt(xrSessionBeginDebugUtilsLabelRegionEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExt(xrSessionEndDebugUtilsLabelRegionEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExt(xrSessionInsertDebugUtilsLabelEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialAnchorMSFT, XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME)
    LinkFunctionExt(xrCreateSpatialAnchorSpaceMSFT, XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME)
    LinkFunctionExt(xrDestroySpatialAnchorMSFT, XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME)
    LinkFunctionExt(xrSetDebugUtilsObjectNameEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExt(xrCreateDebugUtilsMessengerEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyDebugUtilsMessengerEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExt(xrSubmitDebugUtilsMessageEXT, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)
    LinkFunctionExtName(xrSetAndroidApplicationThreadKHR, XR_KHR_android_thread_settings)
    LinkFunctionExt(xrGetVulkanGraphicsDeviceKHR, XR_KHR_VULKAN_ENABLE_EXTENSION_NAME)
    LinkFunctionExtName(xrCreateSwapchainAndroidSurfaceKHR, XR_KHR_android_surface_swapchain)
    LinkFunctionExtName(xrGetOpenGLESGraphicsRequirementsKHR, XR_KHR_opengl_es_enable)
    LinkFunctionExt(xrGetVulkanGraphicsRequirementsKHR, XR_KHR_VULKAN_ENABLE_EXTENSION_NAME)
    LinkFunctionExt(xrGetVulkanInstanceExtensionsKHR, XR_KHR_VULKAN_ENABLE_EXTENSION_NAME)
    LinkFunctionExt(xrGetVulkanDeviceExtensionsKHR, XR_KHR_VULKAN_ENABLE_EXTENSION_NAME)
    LinkFunctionExt(xrCreateVulkanInstanceKHR, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME)
    LinkFunctionExt(xrCreateVulkanDeviceKHR, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME)
    LinkFunctionExt(xrGetVulkanGraphicsDevice2KHR, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME)
    LinkFunctionExt(xrGetVulkanGraphicsRequirements2KHR, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME)
    LinkFunctionExtName(xrCreateBodyTrackerFB, XR_FB_body_tracking)
    LinkFunctionExtName(xrDestroyBodyTrackerFB, XR_FB_body_tracking)
    LinkFunctionExtName(xrLocateBodyJointsFB, XR_FB_body_tracking)
    LinkFunctionExtName(xrGetBodySkeletonFB, XR_FB_body_tracking)
    LinkFunctionExtName(xrShareSpacesFB, XR_FB_spatial_entity_sharing)
    LinkFunctionExtName(xrRequestSceneCaptureFB, XR_FB_scene_capture)
    LinkFunctionExtName(xrGetFoveationEyeTrackedStateMETA, XR_META_foveation_eye_tracked)
    LinkFunctionExtName(xrCreateFaceTrackerFB, XR_FB_face_tracking)
    LinkFunctionExtName(xrDestroyFaceTrackerFB, XR_FB_face_tracking)
    LinkFunctionExtName(xrGetFaceExpressionWeightsFB, XR_FB_face_tracking)
    LinkFunctionExtName(xrCreateEyeTrackerFB, XR_FB_eye_tracking_social)
    LinkFunctionExtName(xrDestroyEyeTrackerFB, XR_FB_eye_tracking_social)
    LinkFunctionExtName(xrGetEyeGazesFB, XR_FB_eye_tracking_social)
    LinkFunctionExtName(xrGetDeviceSampleRateFB, XR_FB_haptic_pcm)
    LinkFunctionExtName(xrEnumerateExternalCamerasOCULUS, XR_OCULUS_external_camera)
    LinkFunctionExtName(xrSaveSpaceListFB, XR_FB_spatial_entity_storage_batch)
    LinkFunctionExtName(xrCreateSpaceUserFB, XR_FB_spatial_entity_user)
    LinkFunctionExtName(xrGetSpaceUserIdFB, XR_FB_spatial_entity_user)
    LinkFunctionExtName(xrDestroySpaceUserFB, XR_FB_spatial_entity_user)
    LinkFunctionExtName(xrSetTrackingOptimizationSettingsHintQCOM, XR_QCOM_tracking_optimization_settings)
    LinkFunctionExt(xrCreatePassthroughHTC, XR_HTC_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrDestroyPassthroughHTC, XR_HTC_PASSTHROUGH_EXTENSION_NAME)
    LinkFunctionExt(xrApplyFoveationHTC, XR_HTC_FOVEATION_EXTENSION_NAME)
    LinkFunctionExtName(xrApplyForceFeedbackCurlMNDX, XR_MNDX_force_feedback_curl)
    LinkFunctionExtName(xrGetPassthroughPreferencesMETA, XR_META_passthrough_preferences)
    LinkFunctionExtName(xrCreateVirtualKeyboardMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrDestroyVirtualKeyboardMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrCreateVirtualKeyboardSpaceMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrSuggestVirtualKeyboardLocationMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrGetVirtualKeyboardScaleMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrSetVirtualKeyboardModelVisibilityMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrGetVirtualKeyboardModelAnimationStatesMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrGetVirtualKeyboardDirtyTexturesMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrGetVirtualKeyboardTextureDataMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrSendVirtualKeyboardInputMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrChangeVirtualKeyboardTextContextMETA, XR_META_virtual_keyboard)
    LinkFunctionExtName(xrCreatePassthroughColorLutMETA, XR_META_passthrough_color_lut)
    LinkFunctionExtName(xrDestroyPassthroughColorLutMETA, XR_META_passthrough_color_lut)
    LinkFunctionExtName(xrUpdatePassthroughColorLutMETA, XR_META_passthrough_color_lut)
    LinkFunctionExtName(xrCreatePlaneDetectorEXT, XR_EXT_plane_detection)
    LinkFunctionExtName(xrDestroyPlaneDetectorEXT, XR_EXT_plane_detection)
    LinkFunctionExtName(xrBeginPlaneDetectionEXT, XR_EXT_plane_detection)
    LinkFunctionExtName(xrGetPlaneDetectionStateEXT, XR_EXT_plane_detection)
    LinkFunctionExtName(xrGetPlaneDetectionsEXT, XR_EXT_plane_detection)
    LinkFunctionExtName(xrGetPlanePolygonBufferEXT, XR_EXT_plane_detection)
    LinkFunctionExtName(xrCreateSpaceFromCoordinateFrameUIDML, XR_ML_compat)
};

FXRFunctionInfo FindXRFunction(const char* FunctionName)
{
    FString Name(FunctionName);
    if (FunctionMapping.Contains(Name))
    {
        FXRFunctionInfo& Function = FunctionMapping[Name];
        return Function;
    }

    return {};
}

bool IsXRFunctionImplemented(const char* FunctionName)
{
    return !StubbedFunctions.Contains(FString(FunctionName));
}

#define MakeStringReturnCase(CaseName, CaseIndex) case CaseName: { return #CaseName; }

FString GetResultString(XrResult Result)
{
    switch (Result)
    {
        XR_LIST_ENUM_XrResult(MakeStringReturnCase)

        default:
        {
            return FormatString("XR_UNKNOWN_%s_%d", Result > 0 ? "SUCCESS" : "FAILURE", Result);
        }
    }
}

#define MakeStructStringReturn(ResultName, ResultIndex) case ResultName: { return #ResultName; }

FString GetStructString(XrStructureType Type)
{
    switch (Type)
    {
        XR_LIST_ENUM_XrStructureType(MakeStringReturnCase)

        default:
        {
            return FormatString("XR_UNKNOWN_STRUCTURE_TYPE_%d", Type);
        }
    }
}
