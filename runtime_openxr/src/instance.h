#pragma once
#include "dll.h"

#include <string>

#include "openxr_includes.h"
#include "system.h"

//TODO fix versioning
#define RUNTIME_VERSION_MAYOR 0
#define RUNTIME_VERSION_MINOR 0
#define RUNTIME_VERSION_PATCH 1

DllExport XrResult xrNegotiateLoaderRuntimeInterface(const XrNegotiateLoaderInfo* loaderInfo, XrNegotiateRuntimeRequest* runtimeRequest);

XrResult xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function);

XrResult xrEnumerateInstanceExtensionProperties(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, XrExtensionProperties* properties);
XrResult xrCreateInstance(const XrInstanceCreateInfo* createInfo, XrInstance* instance);

XrResult xrGetInstanceProperties(XrInstance instance, XrInstanceProperties* instanceProperties);

XrResult xrDestroyInstance(XrInstance instance);

// Directx functions
XrResult xrGetD3D11GraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D11KHR* graphicsRequirements);
XrResult xrGetD3D12GraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D12KHR* graphicsRequirements);

namespace  GameBridge {
    class GB_Instance {
    public:
        // Cannot be longer than XR_MAX_RUNTIME_NAME_SIZE
        const std::string runtime_name = "XR Game Bridge";
        const uint64_t runtime_version = XR_MAKE_VERSION(RUNTIME_VERSION_MAYOR, RUNTIME_VERSION_MINOR, RUNTIME_VERSION_PATCH);
        GB_System system;
    };
}