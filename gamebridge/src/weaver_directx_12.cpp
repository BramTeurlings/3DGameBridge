//
// Created by Bram on 21/06/2023.
//
#pragma once

#include "weaver_directx_12.h"

bool DirectX12Weaver::CreateEffectCopyBuffer(D3D12_RESOURCE_DESC resource_desc) {
    // TODO return the resource somewhere?
    // TODO Check if initialization settings are correct
    // Create resource on the GPU
    ID3D12Resource* intermediateResource;
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;
    device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&intermediateResource));

    return true;
}

GameBridgeManagerType DirectX12Weaver::GetEventManagerType() {
    return GameBridgeManagerType::GB_MANAGER_WEAVER_DX12;
}

DirectX12Weaver::DirectX12Weaver(DX12WeaverInitialize parameters) {
    sr_context = nullptr;
    native_weaver = nullptr;

    input_resource = parameters.input_resource;
    render_target = parameters.render_target;

    parameters.game_bridge->RegisterManager(this);
    command_queue = parameters.command_queue;
    device = parameters.device;
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(command_allocator.GetAddressOf()));
    window = parameters.window;
}

void DirectX12Weaver::InitializeWeaver(SR::SRContext* sr_context) {
    if (native_weaver) {
        delete native_weaver;
    }
    native_weaver = new SR::PredictingDX12Weaver(*sr_context, device.Get(), command_allocator.Get(), command_queue.Get(), input_resource.Get(), render_target.Get(), window, render_target->GetDesc().Format);
}

void DirectX12Weaver::Weave(ID3D12GraphicsCommandList* commandList, unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset) {
    native_weaver->weave(commandList, width, height, xOffset, yOffset);
}

void DirectX12Weaver::SetLatency(int latency_in_microseconds) {
    native_weaver->setLatency(latency_in_microseconds);
}

void DirectX12Weaver::SetInputFrameBuffer(ID3D12Resource* resource, DXGI_FORMAT format)
{
    native_weaver->setInputFrameBuffer(resource, format);
}
