#pragma once
#include "dll.h"

#include <string>

#include "openxr_includes.h"
#include "system.h"
#include "session.h"

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

// Path function
XrResult xrStringToPath(XrInstance instance, const char* pathString, XrPath* path);
XrResult xrPathToString(XrInstance instance, XrPath path, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);

namespace  GameBridge {
    struct GB_ActionSet {
        uint32_t priority;
    };

    inline std::hash<std::string> string_hasher;

    class GB_Instance {
    public:
        // Cannot be longer than XR_MAX_RUNTIME_NAME_SIZE
        const std::string runtime_name = "XR Game Bridge";
        const uint64_t runtime_version = XR_MAKE_VERSION(RUNTIME_VERSION_MAYOR, RUNTIME_VERSION_MINOR, RUNTIME_VERSION_PATCH);

        // TODO maybe we want a list of systems in the future
        GB_System system;
        GB_Session session;

        std::unordered_map<size_t, GB_ActionSet> action_sets;
        std::unordered_map<XrPath, std::string> xrpath_storage;

        /*!
         * \brief Add an action set to the action map
         * \param name Name of the action set, not the localized name
         * \param priority Priority of the action set
         * \return The id of the action set
         */
        size_t AddActionSet(std::string name, uint32_t priority);
    };
}
