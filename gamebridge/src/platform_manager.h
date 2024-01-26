//
// Created by Bram on 08/06/2023.
//

#pragma once

#include <vector>
#include <sr/management/srcontext.h>
#include <d3d12.h>
#include "game_bridge.h"

enum class SRPlatformManagerInitializeFlags {
    SRGB_DISABLE_SERVICE_RECONNECTION,
    SRGB_DISABLE_AUTOMATIC_LENS_SWITCHING
};

enum class WeaverType {
    SRGB_WEAVER_DIRECTX_11,
    SRGB_WEAVER_DIRECTX_12
};

struct SRContextParameters {
    // Window handle
    void* window_handle = nullptr;

    // DirectX 11
    ID3D11DeviceContext* d3d11_device_context = nullptr;
    uint32_t d3d11_width_of_sbs_image = 0;
    uint32_t d3d11_height_of_sbs_image = 0;

    // DirectX 12
    ID3D12Device* d3d12_device = nullptr;
    ID3D12CommandAllocator* d3d12_command_allocator = nullptr;
    ID3D12CommandQueue* d3d12_command_queue = nullptr;
    ID3D12Resource* d3d12_frame_buffer = nullptr;
    ID3D12Resource* d3d12_back_buffer = nullptr;

    // Todo: Implement more members if necessary.
};

// Todo: Do we want this outwards facing code to use a struct too? Won't it be nicer for developers to call the constructor directly? We need to remember that this software's purpose is to very quickly add a simple SR implementation to an existing codebase.
struct SRPlatformManagerInitialize {
    SRContextParameters sr_context_params;
    SR::SRContext *sr_context = nullptr; //Optional
    SRPlatformManagerInitializeFlags flags;
    GameBridge game_bridge;
    WeaverType weaver_type;
};

class PlatformManager : private IGameBridgeManager {
public:
    GameBridge game_bridge;

    void* active_weaver;
    WeaverType weaver_type;

    SR::SRContext contexts[2] = {{}, {}};
    int sr_context_index = 0;

    //EventStreamWriter* event_writer;
    void* event_stream_buffer;

    SRContextParameters sr_context_parameters;

    explicit PlatformManager(SRPlatformManagerInitialize init);

    GameBridgeManagerType GetEventManagerType() override;

    // Returns the SRContext that is currently being used.
    SR::SRContext* GetContext();

    // Returns the currently used. Use the weaver_type to determine what graphics API the weaver is implementing.
    void* GetWeaver();

    void* GetEventBuffer();

private:
    void InitializeSRContext(SRContextParameters sr_context_parameters);
    void ReinitializeContext(SRContextParameters sr_context_parameters);

    void* InitializeWeaver(WeaverType weaver_type);
    void SwitchLens(bool enable);
};
