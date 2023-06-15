//
// Created by Bram on 08/06/2023.
//

#include <map>
#include "hotkeys_interface.h"
#include "../event_system/event_manager.h"
#include "../game_bridge.h"

#ifndef INC_3DGAMEBRIDGE_HOTKEYMANAGER_H
#define INC_3DGAMEBRIDGE_HOTKEYMANAGER_H

#endif //INC_3DGAMEBRIDGE_HOTKEYMANAGER_H
#pragma once

enum class HotkeyManagerInitializeFlags {
    SRGB_HOTKEY_MANAGER_NO_FLAGS
};

struct HotkeyManagerInitialize {
    IHotkeys* implementation;
    HotkeyManagerInitializeFlags flags;
    GameBridge game_bridge;
};

class HotkeyManager : private IGameBridgeManager {
public:
    std::map<uint32_t, GameBridgeEvents> key_combo;
    EventStreamWriter event_stream_writer;
    void* event_stream_buffer;

    explicit HotkeyManager(const EventStreamWriter &event_stream_writer, void* const event_stream);

    GameBridgeManagerType GetEventManagerType() override;

    // Checks if the specified hotkeys are being pressed or not.
    std::map<uint32_t, GameBridgeEvents> PollHotkeys();

    // Adds the specified hotkey to the list of registered hotkeys.
    void AddHotkey(uint32_t key_bits, GameBridgeEvents event_type);

    // Removes the specified hotkey from the list of registered hotkeys.
    void RemoveHotkey(uint32_t key_bits);

    // Uses the event_stream_writer to send all generated events to the EventManager depending on what keys are pressed.
    void SendHotkeyEvents();

    void* GetEventBuffer();
};


