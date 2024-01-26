//
// Created by Bram on 21/06/2023.
//
#pragma once

#include "weaver_directx_12.h"

GameBridgeManagerType DirectX12Weaver::GetEventManagerType() {
    return GameBridgeManagerType::GB_MANAGER_WEAVER_DX12;
}

DirectX12Weaver::DirectX12Weaver(DX12WeaverInitialize dx12_weaver_initialize) {
    back_buffer = dx12_weaver_initialize.in_buffer;

    //Register ourselves as a manager.
    dx12_weaver_initialize.game_bridge->RegisterManager(this);

    // Initialize event stream writer and reader.
    // Get the event stream reader.
    //this->event_stream_reader = dx12_weaver_initialize.game_bridge->GetEventManager().GetEventStream(GameBridgeEventManagerType::GB_MANAGER_EVENT_TYPE_WEAVER);

    // Get the event stream writer.
    //dx12_weaver_initialize.game_bridge->GetEventManager().CreateEventStream(GameBridgeEventManagerType::GB_MANAGER_EVENT_TYPE_WEAVER, &event_stream_writer);
}

// Todo: D3D12_COMMAND_QUEUE_DESC could be the wrong type
void DirectX12Weaver::Weave(D3D12_COMMAND_QUEUE_DESC swap_chain) {}

void DirectX12Weaver::SetLatency(int latency_in_microseconds) {
    native_weavers[native_weaver_index]->setLatency(latency_in_microseconds);
}

void DirectX12Weaver::ReinitializeWeaver(DX12WeaverInitialize weaver_init_stuct) {}

//const void *DirectX12Weaver::GetEventBuffer() {
//    return event_stream_reader->GetEventStream();
//}
