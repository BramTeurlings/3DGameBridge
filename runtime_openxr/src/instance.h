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

// Actions
XrResult xrCreateActionSet(XrInstance instance, const XrActionSetCreateInfo* createInfo, XrActionSet* actionSet);
XrResult xrDestroyActionSet(XrActionSet actionSet);
XrResult xrCreateAction(XrActionSet actionSet, const XrActionCreateInfo* createInfo, XrAction* action);
XrResult xrDestroyAction(XrAction action);

// Suggested bindings
XrResult xrSuggestInteractionProfileBindings(XrInstance instance, const XrInteractionProfileSuggestedBinding* suggestedBindings);

namespace  GameBridge {
    //// Handle functions
    //template<typename T>
    //T PtrToXrHandle() { return T(); };

    //template<typename T>
    //T XrhandleToPtr() { return T(); };

    //template<typename T>
    //T IntToXrHandle() { return T(); };

    //inline size_t XrHandleToInt() { return 0; };

    class GB_Instance {
    public:
        // Cannot be longer than XR_MAX_RUNTIME_NAME_SIZE
        const std::string runtime_name = "XR Game Bridge";
        const uint64_t runtime_version = XR_MAKE_VERSION(RUNTIME_VERSION_MAYOR, RUNTIME_VERSION_MINOR, RUNTIME_VERSION_PATCH);

        // TODO maybe we want a list of systems in the future
        GB_System system;
        GB_Session session;

        std::unordered_map<XrPath, std::string> xrpath_storage;
        // Currently not being used
        XrInteractionProfileSuggestedBinding suggested_bindings;

        /*!
         * \brief Add an action set to the action map
         * \param name Name of the action set, not the localized name
         * \param priority Priority of the action set
         * \return The id of the action set
         */
    };

    struct GB_ActionSet {
        XrInstance instance;
        uint32_t priority;
        std::string localized_name;
    };

    struct GB_Action {
        // TODO list of action sets so multiple action sets can register the same action? Not sure if the specification wants that.
        XrActionSet action_set;
        XrActionType type;
        std::vector<XrPath> sub_action_paths;
        std::string localized_name;
    };

    // Hash class
    inline std::hash<std::string> string_hasher;

    // Data
    // The key is the string hash of an action set path. The same hash is being used for action set handles
    inline std::unordered_map<XrActionSet, GB_ActionSet> action_sets;
    inline std::unordered_map<XrAction, GB_Action> actions;
}
