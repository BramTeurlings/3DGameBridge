#pragma once

#include "game_bridge_manager_interface.h"
#include "event_manager.h"
#include "game_bridge.h"
#include "game_bridge_structs.h"
#include "weaver_interface.h"

#include <sr/weaver/dx12weaver.h>
#include <sr/management/srcontext.h>

#include <wrl/client.h>
#include <directx/d3d12.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct GAME_BRIDGE_API DX12WeaverInitialize{
    GameBridge* game_bridge;
    ID3D12Device* device;
    ID3D12CommandQueue* command_queue;
    ID3D12Resource* input_resource;
    ID3D12Resource* render_target;
    HWND window;
    WeaverFlags flags;
};

class GAME_BRIDGE_API DirectX12Weaver : private GameBridgeWeaver {
    SR::SRContext *sr_context;
    SR::PredictingDX12Weaver* native_weaver;

    ComPtr<ID3D12Resource> input_resource;
    ComPtr<ID3D12Resource> render_target;

    // For (re)initialization purposes
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12CommandQueue> command_queue;
    ComPtr<ID3D12CommandAllocator> command_allocator;

    HWND window;

    // Todo Add event stream

    void SetWeaving(bool weaving_enabled) {}

    bool CreateEffectCopyBuffer(D3D12_RESOURCE_DESC resource_desc);
public:
    GameBridgeManagerType GetEventManagerType() override;

    explicit DirectX12Weaver(DX12WeaverInitialize parameters);

    void InitializeWeaver(SR::SRContext* sr_context) override;
    //void InitializeWeaver(SR::SRContext, WeaverFlags flags);
    //void InitializeWeaver(SR::SRContext, DX12WeaverInitialize params);

    // Todo Maybe not necessary
    //void DestroyWeaver(DX12WeaverInitialize params);

    void Weave(ID3D12GraphicsCommandList* commandList, unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset);

    void SetLatency(int latency_in_microseconds);


    //Not sure we want this
    //const void* GetEventBuffer();
};
