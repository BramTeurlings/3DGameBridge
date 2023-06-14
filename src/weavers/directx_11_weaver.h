//
// Created by Bram on 14/06/2023.
//

// Context
#include <sr/management/srcontext.h>

#include "../event_system/game_event_manager_interface.h"
#include "../event_system/event_manager.h"
#include "../game_bridge.h"
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

public:
    GameBridgeManagerType GetEventManagerType() override {
        return GameBridgeManagerType::SRGB_MANAGER_WEAVER_DX11;
    }

    DirectX11Weaver(DX11WeaverInitialize dx11_weaver_initialize) {
        back_buffer = dx11_weaver_initialize.in_buffer;

        //Register ourselves as a manager.
        dx11_weaver_initialize.game_bridge->RegisterManager(this);

        // Initialize event stream writer and reader.
        // Get the event stream reader.
        this->event_stream_reader = dx11_weaver_initialize.game_bridge->GetEventManager().GetEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER);

        // Get the event stream writer.
        dx11_weaver_initialize.game_bridge->GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_WEAVER, &event_stream_writer);
    }

    // Todo: IDXGISwapChain could be the wrong type
    void Weave(IDXGISwapChain swap_chain) {}

    void SetLatency(int latency_in_microseconds) {
        native_weavers[native_weaver_index]->setLatency(latency_in_microseconds);
    }

    void ReinitializeWeaver(DX11WeaverInitialize weaver_init_stuct) {}

    const void* GetEventBuffer() {
        return event_stream_reader->GetEventStream();
    }
};
