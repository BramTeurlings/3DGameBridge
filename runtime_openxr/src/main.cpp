#include <string>
#include <format>

#include <easylogging++.h>
#include <openxr/openxr.h>

#include "settings.h"

INITIALIZE_EASYLOGGINGPP

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID) {
    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%date %level %loc %msg");

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        char module_path[MAX_PATH];
        GetModuleFileNameA(hInst, module_path, MAX_PATH);
        runtime_path = std::string(module_path);

        FindPathEnv();

        LOG(INFO) << "DLL_PROCESS_ATTACH";

        LOG(INFO) << "XR Game Bridge Loaded";

        LOG(INFO) << "Runtime location: " << module_path;

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

// Targets are delayed in CMake
#include <delayimp.h>
FARPROC WINAPI delayHook(unsigned dliNotify, PDelayLoadInfo pdli) {
#ifdef _DEBUG
    std::string gb_dll_name = "3DGameBridged.dll";
#else
    std::string gb_dll_name = "3DGameBridge.dll";
#endif

    std::string dll_name(pdli->szDll);
    fs::path dll_path = fs::path(sr_install_path) /= "";

    switch (dliNotify) {
    case dliStartProcessing:
        // If you want to return control to the helper, return 0.
        // Otherwise, return a pointer to a FARPROC helper function
        // that will be used instead, thereby bypassing the rest
        // of the helper.

        LOG(INFO) << "dliStartProcessing " << "DLL Name: " << pdli->szDll;

        break;

    case dliNotePreLoadLibrary:
        // If you want to return control to the helper, return 0.
        // Otherwise, return your own HMODULE to be used by the
        // helper instead of having it call LoadLibrary itself.
        LOG(INFO) << "dliNotePreLoadLibrary " << "DLL Name: " << pdli->szDll;

        if(dll_name.find(gb_dll_name) != std::string::npos)
        {
            fs::path gb_path = fs::path(runtime_path).parent_path() /= dll_name;
            HMODULE gb_module = LoadLibraryA(gb_path.string().data());
            if(gb_module == NULL)
            {
                LOG(INFO) << "Failed to load " << gb_path << " error: " << GetLastError();
                return 0;
            }

            return reinterpret_cast<FARPROC>(gb_module);
        }


        break;

    case dliNotePreGetProcAddress:
        // If you want to return control to the helper, return 0.
        // If you choose you may supply your own FARPROC function
        // address and bypass the helper's call to GetProcAddress.
        LOG(INFO) << "dliNotePreGetProcAddress " << "DLL Name: " << pdli->szDll;


        break;

    case dliFailLoadLib:
        // LoadLibrary failed.
        // If you don't want to handle this failure yourself, return 0.
        // In this case the helper will raise an exception
        // (ERROR_MOD_NOT_FOUND) and exit.
        // If you want to handle the failure by loading an alternate
        // DLL (for example), then return the HMODULE for
        // the alternate DLL. The helper will continue execution with
        // this alternate DLL and attempt to find the
        // requested entrypoint via GetProcAddress.

        LOG(INFO) << "dliFailLoadLib " << "DLL Name: " << pdli->szDll;


        break;

    case dliFailGetProc:
        // GetProcAddress failed.
        // If you don't want to handle this failure yourself, return 0.
        // In this case the helper will raise an exception
        // (ERROR_PROC_NOT_FOUND) and exit.
        // If you choose, you may handle the failure by returning
        // an alternate FARPROC function address.
        LOG(INFO) << "dliFailGetProc " << "DLL Name: " << pdli->szDll;


        break;

    case dliNoteEndProcessing:
        // This notification is called after all processing is done.
        // There is no opportunity for modifying the helper's behavior
        // at this point except by longjmp()/throw()/RaiseException.
        // No return value is processed.

        LOG(INFO) << "dliNoteEndProcessing " << "DLL Name: " << pdli->szDll;

        break;

    default:
        LOG(INFO) << "default" << "DLL Name: " << pdli->szDll;
        return NULL;
    }

    return NULL;
}

// and then at global scope somewhere:

ExternC const PfnDliHook __pfnDliNotifyHook2 = delayHook;
ExternC const PfnDliHook __pfnDliFailureHook2 = delayHook;
