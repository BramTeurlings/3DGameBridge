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

class hotkey_manager : IGameBridgeManager {
public:
    std::map<uint32_t, GameBridgeEvents> key_combo;
    EventStreamWriter event_stream_writer;
    void* event_stream_buffer;

    explicit hotkey_manager(const EventStreamWriter &event_stream_writer, void* const event_stream) : event_stream_writer(event_stream_writer), event_stream_buffer(event_stream) {}

    GameBridgeManagerType GetEventManagerType() override {
        return this->game_bridge_manager_type;
    }

    // Checks if the specified hotkeys are being pressed or not.
    std::map<uint32_t, GameBridgeEvents> PollHotkeys() {
        // Not yet implemented, relies on IHotKeys interface.
        // Todo: Implement mechanism that takes key bits from the IHotkeys interface for every registered hotkey and returns which ones are being pressed based on the key_combo map.
        return {};
    }

    // Adds the specified hotkey to the list of registered hotkeys.
    void AddHotkey(uint32_t key_bits, GameBridgeEvents event_type) {
        key_combo.insert(std::map<uint32_t, GameBridgeEvents>::value_type (0, event_type));
    }

    // Removes the specified hotkey from the list of registered hotkeys.
    void RemoveHotkey(uint32_t key_bits) {
        key_combo.erase(key_bits);
    }

    // Uses the event_stream_writer to send all generated events to the EventManager depending on what keys are pressed.
    void SendHotkeyEvents() {
        std::map<uint32_t, GameBridgeEvents> pressed_keys = PollHotkeys();
        for(std::map<uint32_t, GameBridgeEvents>::iterator it = pressed_keys.begin(); it != pressed_keys.end(); it++){
            // Todo: Assuming the uint32_t is always 4 bytes, we use that as the size of the payload. (This is probably wrong?)
            event_stream_writer.SubmitEvent(it->second, 4, (void *)it->first);
        }
    }

    void* GetEventBuffer() {
        return event_stream_buffer;
    }
};


