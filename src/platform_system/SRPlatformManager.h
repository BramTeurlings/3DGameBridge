//
// Created by Bram on 08/06/2023.
//

#include <vector>
#include <sr/management/srcontext.h>
#include "../game_bridge.h"

#ifndef INC_3DGAMEBRIDGE_SRPLATFORMMANAGER_H
#define INC_3DGAMEBRIDGE_SRPLATFORMMANAGER_H

#endif //INC_3DGAMEBRIDGE_SRPLATFORMMANAGER_H
#pragma once

enum class SRPlatformManagerInitializeFlags {
    SRGB_DISABLE_SERVICE_RECONNECTION,
    SRGB_DISABLE_AUTOMATIC_LENS_SWITCHING
};

enum class WeaverType {
    SRGB_WEAVER_DIRECTX_11,
    SRGB_WEAVER_DIRECTX_12
};

struct SRPlatformManagerInitialize {
    //sr_context_params
    SRPlatformManagerInitializeFlags flags;
    GameBridge game_bridge;
    WeaverType weaver_type;
};

class SRPlatformManager {
public:
    void* active_weaver;
    SR::SRContext contexts[2] = {{}, {}};
    int sr_context_index = 0;
    WeaverType weaver_type;
    GameBridge game_bridge;
    EventStreamWriter* event_writer;
    void* event_stream_buffer;

    SRPlatformManager(SRPlatformManagerInitialize init) : game_bridge(init.game_bridge) {
        // Initialize
    }

    // Returns the SRContext that is currently being used.
    SR::SRContext* GetContext() {
        return &contexts[sr_context_index];
    }

    // Returns the currently used. Use the weaver_type to determine what graphics API the weaver is implementing.
    void* GetWeaver() {
        return active_weaver;
    }

    void* GetEventBuffer() {
        return event_stream_buffer;
    }

private:
    //InitializeSRContext(sr_context_params)
    void* InitializeWeaver(WeaverType weaver_type) {};
    void SwitchLens(bool enable) {}
    //void ReinitializeContext(sr_context_params);
};
