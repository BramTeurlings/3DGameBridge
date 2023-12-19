//
// Created by Bram on 14/06/2023.
//

#pragma once

// Context
#include <sr/management/srcontext.h>

#include "game_bridge_manager_interface.h"
#include "event_manager.h"
#include "game_bridge.h"
#include "game_bridge_structs.h"

// Weaver
#include <sr/weaver/dx11weaver.h> // Weaver

// Directx
#include <combaseapi.h>
#include <DirectXMath.h>

struct DX11WeaverInitialize {
    // SDK Params
    ID3D11Device* dev;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swap_chain; // Todo: Do we want to ask this or instead ask for the window handle directly?
    void** in_buffer;
    void** out_buffer;
    WeaverFlags flags;
    GameBridge* game_bridge;
};

class DirectX11Weaver : private IGameBridgeManager {
    SR::SRContext *sr_context;
    SR::PredictingDX11Weaver* native_weavers[2] = { {}, {}};
    int native_weaver_index = 0;
    std::shared_ptr<EventStreamWriter> event_stream_writer;
    std::shared_ptr<EventStreamReader> event_stream_reader;
    uint32_t effect_frame_copy_x = 0, effect_frame_copy_y = 0;
    void** back_buffer;
    bool weaver_initialized = false;
    bool weaving_enabled = false; // Todo: This is only used to prevent a crash on the first frame. Can this be removed?
    bool resize_buffer_failed = false;
    ID3D11Texture2D* texture_copy = nullptr;
    ID3D11ShaderResourceView* resource_copy = nullptr;
    // Todo: Do we need all of these DX objects below?
    ID3D11Device* dx_device = nullptr;
    ID3D11DeviceContext* dx_device_context = nullptr;
    IDXGISwapChain* dx_swap_chain = nullptr;

    void SetWeaving(bool weaving_enabled) {}

    bool create_effect_copy_buffer(ID3D11DeviceContext* device_context, ID3D11RenderTargetView* effect_resource_desc);

    bool init_weaver(ID3D11Device* dev, ID3D11DeviceContext* context, IDXGISwapChain* swap_chain);

public:
    GameBridgeManagerType GetEventManagerType() override;

    DirectX11Weaver(DX11WeaverInitialize dx11_weaver_initialize);

    // Todo: IDXGISwapChain could be the wrong type
    void Weave(IDXGISwapChain* swap_chain);

    void SetLatency(int latency_in_microseconds);

    void ReinitializeWeaver(DX11WeaverInitialize weaver_init_stuct);

    // Not sure we want this
    //const void* GetEventBuffer();
};
