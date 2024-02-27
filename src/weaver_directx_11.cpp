//
// Created by Bram on 21/06/2023.
//
#pragma once

#include "weaver_directx_11.h"

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// Function to create a shader resource view from a texture
ID3D11ShaderResourceView* CreateShaderResourceViewFromTexture(ID3D11Device* pDevice, ID3D11Texture2D* pTexture2D) {
    ID3D11ShaderResourceView* pSRV = nullptr;

    if (pDevice && pTexture2D) {
        // Get the format from the input texture.
        D3D11_TEXTURE2D_DESC pDesc;
        pTexture2D->GetDesc(&pDesc);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = pDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = -1; // Use all mip levels

        // Create the shader resource view
        HRESULT hr = pDevice->CreateShaderResourceView(pTexture2D, &srvDesc, &pSRV);
        if (FAILED(hr)) {
            // Handle error (e.g., log error message, cleanup resources, etc.)
            std::cout << "Error while creating shader resource view from texture." << "\n";
        }
    }

    return pSRV;
}

DirectX11Weaver::DirectX11Weaver(DX11WeaverInitialize dx11_weaver_initialize) {
    back_buffer = dx11_weaver_initialize.in_buffer;

    //Register ourselves as a manager.
    dx11_weaver_initialize.game_bridge->RegisterManager(this);

    // Initialize event stream writer and reader.
    // Get the event stream reader.
    this->event_stream_reader = dx11_weaver_initialize.game_bridge->GetEventManager().GetEventStreamReader(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER);

    // Get the event stream writer.
    dx11_weaver_initialize.game_bridge->GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER, DEFAULT_EVENT_COUNT_MAX, 0);

    dx_device = dx11_weaver_initialize.dev;
    dx_device_context = dx11_weaver_initialize.context;
    dx_swap_chain = dx11_weaver_initialize.swap_chain;
}

GameBridgeManagerType DirectX11Weaver::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_WEAVER_DX11;
}

void DirectX11Weaver::Weave(IDXGISwapChain* swap_chain) {
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swap_chain->GetDesc(&swapChainDesc);
    ID3D11Device* d3d11device;
    swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&d3d11device));

    // Create RTV from back buffer.
    ID3D11RenderTargetView* rtv = nullptr;
    HRESULT hr = d3d11device->CreateRenderTargetView(reinterpret_cast<ID3D11Texture2D*>(back_buffer), nullptr, &rtv);

    if (weaver_initialized) {
        if (FAILED(hr)) {
            // Handle error
            std::cout << "Unable to create render target view from back buffer!" << "\n";
            return;
        }

        //Check texture size
        if (swapChainDesc.BufferDesc.Width != effect_frame_copy_x || swapChainDesc.BufferDesc.Height != effect_frame_copy_y) {
            //TODO Might have to get the buffer from the create_effect_copy_buffer function and only swap them when creation suceeds
            texture_copy->Release();
            resource_copy->Release();
            if (!create_effect_copy_buffer(dx_device_context, rtv) && !resize_buffer_failed) {
                std::cout << "Couldn't create effect copy buffer, trying again next frame" << "\n";
                resize_buffer_failed = true;
            }

            // Set newly created buffer as input
            native_weavers[native_weaver_index]->setInputFrameBuffer(resource_copy);
            std::cout << "Buffer size changed" << "\n";
        }
        else {
            resize_buffer_failed = false;

            if (weaving_enabled) {
                // Copy resource
                create_effect_copy_buffer(dx_device_context, rtv);

                // Bind back buffer as render target
                dx_device_context->OMSetRenderTargets(1, &rtv, 0);

                // Weave to back buffer
                native_weavers[native_weaver_index]->weave(swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);
            }
        }
    }
    else {
        create_effect_copy_buffer(dx_device_context, rtv);
        if (init_weaver(d3d11device, dx_device_context, swap_chain)) {
            // Set context and input frame buffer again to make sure they are correct.
            native_weavers[native_weaver_index]->setContext(dx_device_context);
            native_weavers[native_weaver_index]->setInputFrameBuffer(resource_copy);
        }
        else {
            // When buffer creation succeeds and this fails, delete the created buffer
            texture_copy->Release();
            std::cout << "Failed to initialize weaver." << "\n";
            return;
        }
    }
}

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

// Todo: We may want to change the effect_resource_desc type to ID3D11Resource or texture2D.
bool DirectX11Weaver::create_effect_copy_buffer(ID3D11DeviceContext* device_context, ID3D11RenderTargetView* effect_resource_desc)
{
    // Check if device context or render target view is null
    if (!device_context || !effect_resource_desc)
    {
        return false;
    }

    // Get the render target texture from the render target view
    // GetResource calls AddRef() so we use a ComPtr to make sure it is released after this method finishes.
    ComPtr<ID3D11Resource> sourceResource = nullptr;
    effect_resource_desc->GetResource(&sourceResource);
    if (!sourceResource)
    {
        return false;
    }

    // Query the source texture from the render target texture
    // QueryInterface also calls AddRef() so we need to make sure source_texture is released at some point. This is managed by the ComPtr.
    ComPtr<ID3D11Texture2D> source_texture = nullptr;
    HRESULT hr = sourceResource->QueryInterface(IID_PPV_ARGS(&source_texture));
    if (FAILED(hr) || !source_texture)
    {
        return false;
    }

    // Initialize the destination texture
    texture_copy = nullptr;
    D3D11_TEXTURE2D_DESC texture_desc;

    // Create the destination device
    ComPtr<ID3D11Device> d3d11_device;
    source_texture->GetDesc(&texture_desc);

    // Set the description flags so we can create a shader resource view later.
    texture_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    device_context->GetDevice(&d3d11_device);
    // Create a texture for the copy. We will use this later to make a new ShaderResourceView for the weaver.
    hr = d3d11_device->CreateTexture2D(&texture_desc, nullptr, &texture_copy);
    if (FAILED(hr) || !texture_copy)
    {
        source_texture->Release();
        return false;
    }

    // Todo: I don't think this copy is necessary. I should investigate.
    // Copy the source texture to the destination texture
    // device_context->CopyResource(texture_copy, source_texture.Get());

    // Create shader resource with the description from the source texture.
    resource_copy = CreateShaderResourceViewFromTexture(d3d11_device.Get(), texture_copy);

    return true;
}