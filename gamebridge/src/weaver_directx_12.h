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
#include <sr/weaver/dx12weaver.h> // Weaver

// Directx
#include <combaseapi.h>
#include <DirectXMath.h>
#include "d3d12.h"

struct DX12WeaverInitialize {
    // SDK Params
    D3D12_CPU_DESCRIPTOR_HANDLE* in_buffer;
    D3D12_CPU_DESCRIPTOR_HANDLE* out_buffer;
    GameBridgeWeaverFlags flags;
    GameBridge* game_bridge;
    ID3D12CommandQueue* command_queue;
    ID3D12CommandList* command_list;
};

class DirectX12Weaver : private IGameBridgeManager {
    SR::SRContext *sr_context;
    SR::PredictingDX12Weaver* native_weavers[2] = { {}, {}};
    int native_weaver_index = 0;
    //EventStreamWriter* event_stream_writer;
    //EventStreamReader* event_stream_reader;
    ID3D12CommandQueue* command_queue;
    ID3D12GraphicsCommandList* command_list;

    ID3D12Resource* effect_frame_copy;
    D3D12_CPU_DESCRIPTOR_HANDLE* back_buffer;

    void SetWeaving(bool weaving_enabled) {}

    bool create_effect_copy_buffer(ID3D12Device* device, ID3D12Resource* effect_resource)
    {
        // Check if device or effect resource is null
        if (!device || !effect_resource)
        {
            return false;
        }

        // Create the command queue
        ID3D12CommandQueue* commandQueue;
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

        // Create the command allocator
        ID3D12CommandAllocator* commandAllocator;
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

        // Create the command list
        ID3D12GraphicsCommandList* commandList;
        device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));

        // Transition the effect resource to the appropriate state for copying
        D3D12_RESOURCE_BARRIER barrierDesc = {};
        barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrierDesc.Transition.pResource = effect_resource;
        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
        barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrierDesc);

        // Get the description of the effect resource
        D3D12_RESOURCE_DESC resourceDesc = effect_resource->GetDesc();

        // Create the intermediate resource for the effect frame copy
        ID3D12Resource* intermediateResource;
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CreationNodeMask = 1;
        heapProperties.VisibleNodeMask = 1;
        device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&intermediateResource));

        // Transition the intermediate resource to the appropriate state for copying
        barrierDesc.Transition.pResource = intermediateResource;
        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        commandList->ResourceBarrier(1, &barrierDesc);

        // Copy the effect resource to the intermediate resource
        commandList->CopyResource(intermediateResource, effect_resource);

        // Transition the resources back to their original states
        barrierDesc.Transition.pResource = effect_resource;
        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        commandList->ResourceBarrier(1, &barrierDesc);

        // Close the command list
        commandList->Close();

        // Execute the command list containing our copy commands
        ID3D12CommandList* commandLists[] = { commandList };
        commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        // Wait for the GPU to finish
        ID3D12Fence* fence;
        device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->SetEventOnCompletion(1, fenceEvent);
        commandQueue->Signal(fence, 1);
        WaitForSingleObject(fenceEvent, INFINITE);

        // Clean up resources
        // Todo: Are we not releasing these resources too early? I feel like we are.
        commandQueue->Release();
        commandAllocator->Release();
        commandList->Release();
        intermediateResource->Release();
        fence->Release();

        CloseHandle(fenceEvent);

        return true;
    }

public:
    GameBridgeManagerType GetEventManagerType() override;

    DirectX12Weaver(DX12WeaverInitialize dx12_weaver_initialize);

    // Todo: D3D12_COMMAND_QUEUE_DESC could be the wrong type
    void Weave(D3D12_COMMAND_QUEUE_DESC  swap_chain);

    void SetLatency(int latency_in_microseconds);

    void ReinitializeWeaver(DX12WeaverInitialize weaver_init_stuct);

    //Not sure we want this
    //const void* GetEventBuffer();
};
