#include <string>
#include <format>

#include <easylogging++.h>
#include <openxr/openxr.h>
#include "settings.h"

INITIALIZE_EASYLOGGINGPP

//int main()
//{
//    return 0;
//}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID) {
    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%date %level %loc %msg");

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        LOG(INFO) << "DLL_PROCESS_ATTACH";

        LOG(INFO) << "XR Game Bridge Loaded";

        LOG(INFO) << "Game Bridge: VERSION";

        LOG(INFO) << "OpenXR API: " << std::format("{}.{}.{}", XR_VERSION_MAJOR(XR_CURRENT_API_VERSION), XR_VERSION_MINOR(XR_CURRENT_API_VERSION), XR_VERSION_PATCH(XR_CURRENT_API_VERSION));

        LOG(INFO) << "Process: ";
        LOG(INFO) << "Executable: ";

        LOG(INFO) << "Support D3D11 " << (XRGameBridge::g_runtime_settings.support_d3d11 ? "TRUE" : "FALSE");
        LOG(INFO) << "Support D3D12 " << (XRGameBridge::g_runtime_settings.support_d3d12 ? "TRUE" : "FALSE");
        LOG(INFO) << "Support GL " << (XRGameBridge::g_runtime_settings.support_gl ? "TRUE" : "FALSE");
        LOG(INFO) << "Support VK " << (XRGameBridge::g_runtime_settings.support_vk ? "TRUE" : "FALSE");

        //if (FClientSettings::ClientSettings.AllowVK)
        //{
        //    int Status = gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
        //    Log(FLogOpenXRInterface, Trace, "GLAD VK status: %i", Status);
        //}

        XRGameBridge::g_runtime_settings.hInst = hInst;

        break;
    }

    case DLL_PROCESS_DETACH:
    {
        LOG(INFO) << "DLL_PROCESS_DETACH";

        LOG(INFO) << "XR Game Bridge Unloaded";
        break;
    }

    default: { break; }
    }
    return TRUE;
}
