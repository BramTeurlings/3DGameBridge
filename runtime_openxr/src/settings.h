#pragma once
#include <regex>
#include <windows.h>
#include <string>
#include <easylogging++.h>
#include <filesystem>
#include  <array>

namespace fs = std::filesystem;

namespace XRGameBridge {
    struct GB_RuntimeSettings {
        bool support_d3d12 = true;
        bool support_d3d11 = false;
        bool support_vk = false;
        bool support_gl = false;
        HINSTANCE hInst;
    } inline g_runtime_settings;
}

constexpr std::array sr_dlls = {
"SimulatedRealityDirectX.dll",
"SimulatedReality.dll",
"SimulatedRealityCore.dll",
"SimulatedRealityCameras.dll",
"SimulatedRealityHandTrackers.dll",
"SimulatedRealityFaceTrackers.dll",
"SimulatedRealityUserModelers.dll",
"SimulatedRealityDisplays.dll",
"DimencoWeaving.dll",
"glog.dll",
"LeapC.dll",
"opencv_world343.dll"
};

inline std::string sr_install_path;
inline std::string sr_install_path_win32;
inline std::string runtime_path;
static void FindPathEnv() {
    std::string path_environment_variable = std::getenv("PATH");
    std::regex path_search_regex("[a-zA-Z0-9+_\\-\\.:%()\\s\\\\]+");

    auto words_begin = std::sregex_iterator(path_environment_variable.begin(), path_environment_variable.end(), path_search_regex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        LOG(INFO) << match_str << '\n';
        if (match_str.find("Simulated Reality") != std::string::npos) {
            if (match_str.find("x86") != std::string::npos) {
                sr_install_path_win32 = match_str;
            }
            else {
                sr_install_path = match_str;
            }
        }
    }
}
