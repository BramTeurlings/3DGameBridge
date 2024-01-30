#pragma once
#include "dll.h"

#include <string>

#include "openxr_includes.h"

DllExport XrResult xrNegotiateLoaderRuntimeInterface(const XrNegotiateLoaderInfo* loaderInfo, XrNegotiateRuntimeRequest* runtimeRequest);

XrResult xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function);

XrResult xrEnumerateInstanceExtensionProperties(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, XrExtensionProperties* properties);
XrResult xrCreateInstance(const XrInstanceCreateInfo* createInfo, XrInstance* instance);
XrResult xrDestroyInstance(XrInstance instance);
XrResult xrGetInstanceProperties(XrInstance instance, XrInstanceProperties* instanceProperties);

// Directx functions
XrResult xrGetD3D11GraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D11KHR* graphicsRequirements);
XrResult xrGetD3D12GraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D12KHR* graphicsRequirements);

namespace  GameBridge {
    class GB_Instance {
    public:
        std::string name;
    };
}