#include "instance.h"

#include <stdexcept>
#include <vector>
#include <easylogging++.h>
#include <set>

#include "openxr_functions.h"

//class OpenXRContainers {
//public:
//    std::vector<XrInstance> instance_handles;
//    std::vector<GBInstance*> instances;
//
////public:
////    void Add(XrInstance instance, GBInstance* gb_instance);
////    const GBInstance* GetGameBridgeInstance(XrInstance);
//
//} static g_openxr_stuff;

using namespace GameBridge;

GB_Instance* g_gbinstance = nullptr;


XrResult xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function) {
    try {
        *function = GameBridge::openxr_functions.at(name);
    }
    catch (std::out_of_range& e) {
        return XR_ERROR_FUNCTION_UNSUPPORTED;
    }
    catch (std::exception& e) {
        return XR_ERROR_RUNTIME_FAILURE; // Generic error
    }

    LOG(INFO) << "Address retrieved: " << name;
    return XR_SUCCESS;
}

XrResult xrNegotiateLoaderRuntimeInterface(const XrNegotiateLoaderInfo* loaderInfo, XrNegotiateRuntimeRequest* runtimeRequest) {
    LOG(INFO) << "xrNegotiateLoaderRuntimeInterface";
    LOG(INFO) << "\tminApiVersion " << loaderInfo->minApiVersion;
    LOG(INFO) << "\tmaxApiVersion " << loaderInfo->maxApiVersion;

    runtimeRequest->runtimeApiVersion = XR_CURRENT_API_VERSION;
    runtimeRequest->runtimeInterfaceVersion = XR_CURRENT_LOADER_RUNTIME_VERSION;
    runtimeRequest->getInstanceProcAddr = &xrGetInstanceProcAddr;

    LOG(INFO) << "\truntimeApiVersion %i", runtimeRequest->runtimeApiVersion;
    LOG(INFO) << "\truntimeInterfaceVersion %i", runtimeRequest->runtimeInterfaceVersion;

    return XR_SUCCESS;
}

XrResult xrEnumerateInstanceExtensionProperties(const char* layerName, uint32_t propertyCapacityInput, uint32_t* propertyCountOutput, XrExtensionProperties* properties) {
    LOG(INFO) << "Called: xrEnumerateInstanceExtensionProperties";
    const uint32_t array_size = static_cast<uint32_t>(supported_extensions.size());

    *propertyCountOutput = array_size;

    // Request for the extension array or the extension array itself
    if (layerName == nullptr && propertyCapacityInput == 0) {
        return XR_SUCCESS;
    }
    // Passed array not large enough
    else if (propertyCapacityInput < array_size) {
        return XR_ERROR_SIZE_INSUFFICIENT;
    }
    // Return whether the extension exists
    else {
        // Fill array
        memcpy_s(properties, propertyCapacityInput * sizeof(XrExtensionProperties), supported_extensions.data(), array_size * sizeof(XrExtensionProperties));
        return XR_SUCCESS;
    }
}

XrResult xrCreateInstance(const XrInstanceCreateInfo* createInfo, XrInstance* instance) {
    if (createInfo == nullptr) {
        LOG(INFO) << "Invalid XrInstanceCreateInfo";
        return XR_ERROR_INITIALIZATION_FAILED;
    }

    if (createInfo->type != XR_TYPE_INSTANCE_CREATE_INFO) {
        LOG(INFO) << "createInfo struct type not XR_TYPE_INSTANCE_CREATE_INFO";
        return XR_ERROR_INITIALIZATION_FAILED;
    }

    // Only support a single instance for now
    if (g_gbinstance != nullptr) {
        return XR_ERROR_LIMIT_REACHED;
    }

    // Check application info
    XrApplicationInfo app_info = createInfo->applicationInfo;
    std::string app_name = app_info.applicationName;

    LOG(INFO) << "Application name: "       << app_name;
    LOG(INFO) << "Api version: "            << app_info.apiVersion;
    LOG(INFO) << "Application version: "    << app_info.applicationVersion;
    LOG(INFO) << "Engine: "                 << app_info.engineName;
    LOG(INFO) << "Engine version: "         << app_info.engineVersion;

    if (app_name.empty()) {
        return XR_ERROR_NAME_INVALID;
    }

    if (app_info.apiVersion != XR_CURRENT_API_VERSION) {
        return XR_ERROR_API_VERSION_UNSUPPORTED;
    }

    // Check api layers
    const char* const* api_layers = createInfo->enabledApiLayerNames;

    // Check extensions
    const char* const* api_extensions = createInfo->enabledExtensionNames;
    // Create set of application extensions
    std::set <std::string > application_extensions;
    for(uint32_t i  = 0; i < createInfo->enabledExtensionCount; i++)
    {
        application_extensions.insert(application_extensions.end(), api_extensions[i]);
    }
    // Remove our extensions from application extensions
    for (auto extension : supported_extensions)
    {
        application_extensions.erase(extension.extensionName);
    }
    if(!application_extensions.empty())
    {
        LOG(ERROR) << "Unsupported extensions: ";
        for(auto extension : application_extensions)
        {
            LOG(ERROR) << "\t" << extension;
        }

        return XR_ERROR_EXTENSION_NOT_PRESENT;
    }


    // Create new instance
    g_gbinstance = new GB_Instance();
    g_gbinstance->name = "Game Bridge";

    const auto address = reinterpret_cast<size_t>(&g_gbinstance);
    *instance = reinterpret_cast<XrInstance>(address);

    LOG(INFO) << "New GameBridge Instance created";
    return XR_SUCCESS;
}

XrResult xrDestroyInstance(XrInstance instance) {
    return XR_SUCCESS;
}

XrResult xrGetInstanceProperties(XrInstance instance, XrInstanceProperties* instanceProperties) {
    return XR_SUCCESS;
}

XrResult xrGetD3D11GraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D11KHR* graphicsRequirements)
{
    return XR_SUCCESS;
}

XrResult xrGetD3D12GraphicsRequirementsKHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D12KHR* graphicsRequirements)
{
    return XR_SUCCESS;
}
