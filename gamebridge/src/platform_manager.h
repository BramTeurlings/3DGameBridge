#pragma once

#include "game_bridge.h"
#include <sr/management/srcontext.h>

enum class SRPlatformManagerInitializeFlags {
    SRGB_DISABLE_SERVICE_RECONNECTION,
    SRGB_DISABLE_AUTOMATIC_LENS_SWITCHING
};

enum class WeaverType {
    SRGB_WEAVER_DIRECTX_11,
    SRGB_WEAVER_DIRECTX_12
};

//struct SRContextParameters {
//    // Window handle
//    void* window_handle = nullptr;
//
//    // DirectX 11
//    ID3D11DeviceContext* d3d11_device_context = nullptr;
//    uint32_t d3d11_width_of_sbs_image = 0;
//    uint32_t d3d11_height_of_sbs_image = 0;
//
//    // DirectX 12
//    ID3D12Device* d3d12_device = nullptr;
//    ID3D12CommandAllocator* d3d12_command_allocator = nullptr;
//    ID3D12CommandQueue* d3d12_command_queue = nullptr;
//    ID3D12Resource* d3d12_frame_buffer = nullptr;
//    ID3D12Resource* d3d12_back_buffer = nullptr;
//
//    // Todo: Implement more members if necessary.
//};

struct SRPlatformManagerInitialize {
    SR::SRContext *sr_context = nullptr; //Optional
    SRPlatformManagerInitializeFlags flags;
    GameBridge game_bridge;
    WeaverType weaver_type;
};

class PlatformManager : private IGameBridgeManager {
public:
    WeaverType weaver_type;
    // Todo Create event stream

    SR::SRContext* sr_context;
    SR::SwitchableLensHint* lens_hint;

    explicit PlatformManager(SRPlatformManagerInitialize parameters);

    SR::SRContext* GetContext();
    GameBridgeManagerType GetEventManagerType() override;

private:
    bool InitializeSRContext();
    void SwitchLens(bool enable);
};
