//
// Created by Bram on 15/06/2023.
//

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

// Todo: implement this
void *PlatformManager::InitializeWeaver(WeaverType weaver_type) {
    return nullptr;
}

void PlatformManager::SwitchLens(bool enable) {}

GameBridgeManagerType PlatformManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_PLATFORM;
}
