#include <string>
#include <format>

#include <easylogging++.h>
//

#include <openxr/openxr.h>

INITIALIZE_EASYLOGGINGPP


//int main()
//{
//    return 0;
//}

struct RuntimeSettings {
    bool support_d3d12 = true;
    bool support_d3d11 = true;
    bool support_vk = false;
    bool support_gl = false;
} static g_runtime_settings;


BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        LOG(INFO) << "DLL_PROCESS_ATTACH";

        LOG(INFO) << "SR Hydra loaded";

        LOG(INFO) << "Game Bridge: VERSION";

        LOG(INFO) << "OpenXR API: " << std::format("{}.{}.{}", XR_VERSION_MAJOR(XR_CURRENT_API_VERSION), XR_VERSION_MINOR(XR_CURRENT_API_VERSION), XR_VERSION_PATCH(XR_CURRENT_API_VERSION));

        LOG(INFO) << "Process: ";
        LOG(INFO) << "Executable: ";

        LOG(INFO) << "Support D3D11 " << (g_runtime_settings.support_d3d11 ? "TRUE" : "FALSE");
        LOG(INFO) << "Support D3D12 " << (g_runtime_settings.support_d3d12 ? "TRUE" : "FALSE");
        LOG(INFO) << "Support GL " << (g_runtime_settings.support_gl ? "TRUE" : "FALSE");
        LOG(INFO) << "Support VK " << (g_runtime_settings.support_vk ? "TRUE" : "FALSE");

        //if (FClientSettings::ClientSettings.AllowVK)
        //{
        //    int Status = gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
        //    Log(FLogOpenXRInterface, Trace, "GLAD VK status: %i", Status);
        //}

        break;
    }

    case DLL_PROCESS_DETACH:
    {
        LOG(INFO) << "DLL_PROCESS_DETACH";

        LOG(INFO) << "Game Bridge unloaded";
        break;
    }

    default: { break; }
    }
    return TRUE;
}
