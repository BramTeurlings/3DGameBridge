//
// Created by Bram on 21/06/2023.
//
#pragma once

#include "weaver_directx_11.h"

#include <sr/utility/exception.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// Function to create a shader resource view from a texture
ID3D11ShaderResourceView* CreateShaderResourceViewFromTexture(ComPtr<ID3D11Device> p_device, ComPtr<ID3D11Texture2D> p_texture2D) {
    ID3D11ShaderResourceView* pSRV = nullptr;

    if (p_device && p_texture2D) {
        // Get the format from the input texture.
        D3D11_TEXTURE2D_DESC pDesc;
        p_texture2D->GetDesc(&pDesc);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = pDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = -1; // Use all mip levels

        // Create the shader resource view
        HRESULT hr = p_device->CreateShaderResourceView(p_texture2D.Get(), &srvDesc, &pSRV);
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

    create_sr_context();
    weaving_enabled = true;
}

GameBridgeManagerType DirectX11Weaver::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_WEAVER_DX11;
}

bool DirectX11Weaver::ResizeIfBufferSizeChanged(DXGI_SWAP_CHAIN_DESC swap_chain_desc, ComPtr<ID3D11Texture2D> current_back_buffer) {
    if (swap_chain_desc.BufferDesc.Width != effect_frame_copy_x || swap_chain_desc.BufferDesc.Height != effect_frame_copy_y) {
        // Update the frame copy width/height
        effect_frame_copy_x = swap_chain_desc.BufferDesc.Width;
        effect_frame_copy_y = swap_chain_desc.BufferDesc.Height;
        //TODO Might have to get the buffer from the create_effect_copy_buffer function and only swap them when creation succeeds
        texture_copy.Reset();
        resource_copy.Reset();
        if (!create_effect_copy_buffer(dx_device, current_back_buffer) && !resize_buffer_failed) {
            std::cout << "Couldn't create effect copy buffer, trying again next frame" << "\n";
            resize_buffer_failed = true;
        }

        // Set newly copied buffer as the input for the weaver to weave on
        native_weavers[native_weaver_index]->setInputFrameBuffer(resource_copy.Get());
        std::cout << "Buffer size changed" << "\n";
        return true;
    }

    return false;
}

void DirectX11Weaver::DoWeave(ComPtr<ID3D11Texture2D> current_back_buffer, ComPtr<ID3D11RenderTargetView> rtv, DXGI_SWAP_CHAIN_DESC swap_chain_desc, bool use_copy_buffer) {
    if (weaving_enabled) {
        // Copy resource
        // Todo: Check if we should only do this once and on resize, then move this function elsewhere.
        if (use_copy_buffer) {
            create_effect_copy_buffer(dx_device, current_back_buffer);
        }

        // Bind back buffer as render target
        dx_device_context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);

        // Weave to back buffer
        native_weavers[native_weaver_index]->weave(swap_chain_desc.BufferDesc.Width, swap_chain_desc.BufferDesc.Height);
    }
}

void DirectX11Weaver::Weave(IDXGISwapChain* swap_chain, bool use_copy_buffer_and_handle_resize) {
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swap_chain->GetDesc(&swapChainDesc);
    ComPtr<ID3D11Device> d3d11device;
    swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(d3d11device.GetAddressOf()));

    // Create RTV from back buffer.
    ComPtr<ID3D11RenderTargetView> rtv = nullptr;
    ComPtr<ID3D11Texture2D> current_back_buffer = reinterpret_cast<ID3D11Texture2D*>(back_buffer);

    // Create the RTV with the back_buffer which we will copy for the weaver later
    HRESULT hr = d3d11device->CreateRenderTargetView(current_back_buffer.Get(), nullptr, &rtv);

    if (weaver_initialized) {
        if (FAILED(hr)) {
            // Handle error
            std::cout << "Unable to create render target view from back buffer!" << "\n";
            return;
        }

        // Check if we need to copy the buffer
        if (use_copy_buffer_and_handle_resize) {
            // Check texture size, if it hasn't changed then do the standard weaving behavior
            if (!ResizeIfBufferSizeChanged(swapChainDesc, current_back_buffer)) {
                // No resize occurred
                resize_buffer_failed = false;

                // Weave logic
                DoWeave(current_back_buffer, rtv, swapChainDesc, true);
            }
        }
        else if (weaving_enabled) {
            // Weave logic
            DoWeave(current_back_buffer, rtv, swapChainDesc, true);
        }
    }
    else {
        bool effect_copy_buffer_succeeded = false;
        if (use_copy_buffer_and_handle_resize) {
            texture_copy.Reset();
            if (create_effect_copy_buffer(dx_device, current_back_buffer)) {
                effect_copy_buffer_succeeded = true;
            }
        }

        if (init_weaver(d3d11device.Get(), dx_device_context.Get(), swap_chain)) {
            // Set context and input frame buffer again to make sure they are correct.
            native_weavers[native_weaver_index]->setContext(dx_device_context.Get());
            if (use_copy_buffer_and_handle_resize) {
                native_weavers[native_weaver_index]->setInputFrameBuffer(resource_copy.Get());
            }
        }
        else if (use_copy_buffer_and_handle_resize && effect_copy_buffer_succeeded) {
            // When buffer creation succeeds and weaver initalization fails, delete the created buffer
            texture_copy.Reset();
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

bool DirectX11Weaver::create_sr_context() {
    if (sr_context == nullptr) {
        try {
            sr_context = new SR::SRContext();
            return true;
        }
        catch (SR::ServerNotAvailableException& e) {
            std::cout << "Server not available, trying again later." << std::endl;
        }
    }
    return false;
}

// Todo: We should make a separate call to this only for modding and also have a different minimal weave call.
bool DirectX11Weaver::create_effect_copy_buffer(ComPtr<ID3D11Device> p_device, ComPtr<ID3D11Texture2D> p_current_back_buffer)
{
    // Check if device context or render target view is null
    if (!p_device || !p_current_back_buffer)
    {
        return false;
    }

    // Todo: We don't have to create a new texture every frame
    // Create a texture for the copy. We will use this later to make a new ShaderResourceView for the weaver.
    if (texture_copy == nullptr) {
        // Initialize the destination texture
        D3D11_TEXTURE2D_DESC texture_desc;
        p_current_back_buffer->GetDesc(&texture_desc);

        // Set the description flags, so we can create a shader resource view later.
        texture_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = p_device->CreateTexture2D(&texture_desc, nullptr, &texture_copy);

        if (FAILED(hr) || !texture_copy)
        {
            return false;
        }
    }

    // Create shader resource with the description from the source texture.
    resource_copy = CreateShaderResourceViewFromTexture(p_device, texture_copy);

    return true;
}