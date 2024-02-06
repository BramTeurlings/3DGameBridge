#pragma once
#include <windows.h>

namespace GameBridge {
    struct GB_RuntimeSettings {
        bool support_d3d12 = true;
        bool support_d3d11 = false;
        bool support_vk = false;
        bool support_gl = false;
        HINSTANCE hInst;
    } inline g_runtime_settings;
}
