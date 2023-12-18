//
// Created by Bram on 21/06/2023.
//
#pragma once

#include "weaver_directx_11.h"

DirectX11Weaver::DirectX11Weaver(DX11WeaverInitialize dx11_weaver_initialize) {
    back_buffer = dx11_weaver_initialize.in_buffer;

    //Register ourselves as a manager.
    dx11_weaver_initialize.game_bridge->RegisterManager(this);

    // Initialize event stream writer and reader.
    // Get the event stream reader.
    this->event_stream_reader = dx11_weaver_initialize.game_bridge->GetEventManager().GetEventStreamReader(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER);

    // Get the event stream writer.
    dx11_weaver_initialize.game_bridge->GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER, DEFAULT_EVENT_COUNT_MAX, 0);
}

GameBridgeManagerType DirectX11Weaver::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_WEAVER_DX11;
}

void DirectX11Weaver::Weave(IDXGISwapChain* swap_chain) {}

void DirectX11Weaver::SetLatency(int latency_in_microseconds) {
    native_weavers[native_weaver_index]->setLatency(latency_in_microseconds);
}

void DirectX11Weaver::ReinitializeWeaver(DX11WeaverInitialize weaver_init_stuct) {}

//const void *DirectX11Weaver::GetEventBuffer() {
//    return event_stream_reader->GetEventStream();
//}

bool DirectX11Weaver::init_weaver(ID3D11Device* dev, ID3D11DeviceContext* context, IDXGISwapChain* swap_chain) {
    if (weaver_initialized) {
        return weaver_initialized;
    }

    delete native_weavers[native_weaver_index];
    native_weavers[native_weaver_index] = nullptr;

//    reshade::api::resource_desc desc = d3d11device->get_resource_desc(rtv);
//    ID3D11Device* dev = (ID3D11Device*)d3d11device->get_native();
//    ID3D11DeviceContext* context = (ID3D11DeviceContext*)cmd_list->get_native();

    if (!dev) {
        std::cout << "Couldn't get a d3d11 device during weaver initialization." << "\n";
        return false;
    }

    if (!context) {
        std::cout << "Couldn't get a d3d11 device context during weaver initialization." << "\n";
        return false;
    }

    try {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        swap_chain->GetDesc(&swapChainDesc);

        native_weavers[native_weaver_index] = new SR::PredictingDX11Weaver(*sr_context, dev, context, swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height, swapChainDesc.OutputWindow);
        native_weavers[native_weaver_index]->setContext(context);
        native_weavers[native_weaver_index]->setInputFrameBuffer((ID3D11ShaderResourceView*)back_buffer); //resourceview of the buffer
        sr_context->initialize();
        std::cout <<  "Initialized weaver" << "\n";
    }
    catch (std::exception e) {
        std::cout <<  "Error during weaver initialization: \n" <<  e.what() << "\n";
        return false;
    }
    catch (...) {
        std::cout <<  "Error during weaver initialization";
        return false;
    }

    weaver_initialized = true;
    return weaver_initialized;
}

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