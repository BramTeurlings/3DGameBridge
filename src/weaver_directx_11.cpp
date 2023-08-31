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
    //this->event_stream_reader = dx11_weaver_initialize.game_bridge->GetEventManager().GetEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER);

    // Get the event stream writer.
    //dx11_weaver_initialize.game_bridge->GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER, &event_stream_writer);
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
