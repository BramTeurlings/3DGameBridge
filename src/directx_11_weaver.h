//
// Created by Bram on 14/06/2023.
//

// Context
#include <sr/management/srcontext.h>

#include "manager_interface.h"
#include "event_manager.h"
#include "game_bridge.h"
#include "weaver_flags.h"

// Weaver
#include <sr/weaver/dx11weaver.h> // Weaver

// Directx
#include <combaseapi.h>
#include <DirectXMath.h>

#ifndef INC_3DGAMEBRIDGE_DIRECTX_11_WEAVER_H
#define INC_3DGAMEBRIDGE_DIRECTX_11_WEAVER_H

#endif //INC_3DGAMEBRIDGE_DIRECTX_11_WEAVER_H
#pragma once

struct DX11WeaverInitialize {
    // SDK Params
    void** in_buffer;
    void** out_buffer;
    WeaverFlags flags;
    GameBridge* game_bridge;
};

class DirectX11Weaver : private IGameBridgeManager {
    SR::SRContext *sr_context;
    SR::PredictingDX11Weaver* native_weavers[2] = { {}, {}};
    int native_weaver_index = 0;
    EventStreamWriter* event_stream_writer;
    EventStreamReader* event_stream_reader;
    void** back_buffer;

    void SetWeaving(bool weaving_enabled) {}

    bool DirectX11Weaver::create_effect_copy_buffer(ID3D11DeviceContext* device_context, ID3D11RenderTargetView* effect_resource_desc)
    {
        // Check if device context or render target view is null
        if (!device_context || !effect_resource_desc)
        {
            return false;
        }

        // Get the render target texture from the render target view
        ID3D11Resource* sourceResource = nullptr;
        effect_resource_desc->GetResource(&sourceResource);
        if (!sourceResource)
        {
            return false;
        }

        // Query the source texture from the render target texture
        ID3D11Texture2D* sourceTexture = nullptr;
        HRESULT hr = sourceResource->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(&sourceTexture));
        sourceResource->Release();
        if (FAILED(hr) || !sourceTexture)
        {
            return false;
        }

        // Create the destination texture
        ID3D11Texture2D* destinationTexture = nullptr;
        D3D11_TEXTURE2D_DESC textureDesc;
        // Create the destination device
        ID3D11Device* d3d11_device;
        sourceTexture->GetDesc(&textureDesc);
        device_context->GetDevice(&d3d11_device);
        hr = d3d11_device->CreateTexture2D(&textureDesc, nullptr, &destinationTexture);
        if (FAILED(hr) || !destinationTexture)
        {
            sourceTexture->Release();
            return false;
        }

        // Copy the source texture to the destination texture
        device_context->CopyResource(destinationTexture, sourceTexture);

        // Release the source and destination textures
        sourceTexture->Release();
        destinationTexture->Release();

        return true;
    }

public:
    GameBridgeManagerType GetEventManagerType() override;

    DirectX11Weaver(DX11WeaverInitialize dx11_weaver_initialize);

    // Todo: IDXGISwapChain could be the wrong type
    void Weave(IDXGISwapChain* swap_chain);

    void SetLatency(int latency_in_microseconds);

    void ReinitializeWeaver(DX11WeaverInitialize weaver_init_stuct);

    const void* GetEventBuffer();
};
