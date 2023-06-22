//
// Created by Bram on 15/06/2023.
//

#include <sr/weaver/dx11weaver.h>
#include <sr/weaver/dx12weaver.h>
#include "platform_manager.h"

PlatformManager::PlatformManager(SRPlatformManagerInitialize init) : game_bridge(init.game_bridge) {
    // Initialize
}

SR::SRContext *PlatformManager::GetContext() {
    return &contexts[sr_context_index];
}

void *PlatformManager::GetWeaver() {
    return active_weaver;
}

void *PlatformManager::GetEventBuffer() {
    return event_stream_buffer;
}

// Todo: Requires a DeviceContext in DX11 or a device and CommandAllocator/Queue in DX12. Is it ok to get these from the manager? How do we deal with them becoming invalid since the platform manager is not the owner?
void *PlatformManager::InitializeWeaver(WeaverType weaver_type) {
    switch (weaver_type) {
        case WeaverType::SRGB_WEAVER_DIRECTX_11:
            ID3D11Device* device;
            sr_context_parameters.d3d11_device_context->GetDevice(&device);
            this->active_weaver = new SR::PredictingDX11Weaver(contexts[sr_context_index], device, sr_context_parameters.d3d11_device_context, sr_context_parameters.d3d11_width_of_sbs_image, sr_context_parameters.d3d11_height_of_sbs_image, (HWND)sr_context_parameters.window_handle);
        case WeaverType::SRGB_WEAVER_DIRECTX_12:
            this->active_weaver = new SR::PredictingDX12Weaver(contexts[sr_context_index], sr_context_parameters.d3d12_device, sr_context_parameters.d3d12_command_allocator, sr_context_parameters.d3d12_command_queue, sr_context_parameters.d3d12_frame_buffer, sr_context_parameters.d3d12_back_buffer, (HWND)sr_context_parameters.window_handle);
    }

    return active_weaver;
}

void PlatformManager::SwitchLens(bool enable) {}

GameBridgeManagerType PlatformManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_PLATFORM;
}

void PlatformManager::InitializeSRContext(SRContextParameters sr_context_parameters) {
    // Todo: Implement
}

void PlatformManager::ReinitializeContext(SRContextParameters sr_context_parameters) {
    // Todo: Implement
}
