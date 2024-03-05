//
// Created by Bram on 15/06/2023.
//

#include "sr/sense/display/switchablehint.h"
#include "sr/sense/core/inputstream.h"
#include "sr/utility/exception.h"
#include "sr/types.h"

#include "platform_manager.h"

PlatformManager::PlatformManager(SRPlatformManagerInitialize parameters) : sr_context(nullptr), lens_hint(nullptr) {

}

SR::SRContext* PlatformManager::GetContext() {
    return sr_context;
}

// Todo Might be better to just do this inside the weaver/ a weaver manager itself
// Todo: Requires a DeviceContext in DX11 or a device and CommandAllocator/Queue in DX12. Is it ok to get these from the manager? How do we deal with them becoming invalid since the platform manager is not the owner?
//void *PlatformManager::InitializeWeaver(WeaverType weaver_type) {
//    switch (weaver_type) {
//        case WeaverType::SRGB_WEAVER_DIRECTX_11:
//            ID3D11Device* device;
//            sr_context_parameters.d3d11_device_context->GetDevice(&device);
//            this->active_weaver = new SR::PredictingDX11Weaver(contexts[sr_context_index], device, sr_context_parameters.d3d11_device_context, sr_context_parameters.d3d11_width_of_sbs_image, sr_context_parameters.d3d11_height_of_sbs_image, (HWND)sr_context_parameters.window_handle);
//        case WeaverType::SRGB_WEAVER_DIRECTX_12:
//            this->active_weaver = new SR::PredictingDX12Weaver(contexts[sr_context_index], sr_context_parameters.d3d12_device, sr_context_parameters.d3d12_command_allocator, sr_context_parameters.d3d12_command_queue, sr_context_parameters.d3d12_frame_buffer, sr_context_parameters.d3d12_back_buffer, (HWND)sr_context_parameters.window_handle);
//    }
//
//    return active_weaver;
//}

void PlatformManager::SwitchLens(bool enable) {}

GameBridgeManagerType PlatformManager::GetEventManagerType() {
    return GameBridgeManagerType::GB_MANAGER_PLATFORM;
}

bool PlatformManager::InitializeSRContext() {
    // Todo retry mechanism
    if (sr_context == nullptr) {
        try {
            sr_context = new SR::SRContext(false);
        }
        catch (SR::ServerNotAvailableException& ex) {
            // Unable to construct SR Context.
            return false;
        }
        screen = SR::Screen::create(*sr_context);
        lens_hint = SR::SwitchableLensHint::create(*sr_context);
        sr_context->initialize();
    }
}

SR::Screen* PlatformManager::GetScreen() {
    return screen;
}

SR::SwitchableLensHint* PlatformManager::GetLensHint() {
    return lens_hint;
}
