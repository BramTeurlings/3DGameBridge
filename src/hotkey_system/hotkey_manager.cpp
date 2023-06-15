//
// Created by Bram on 15/06/2023.
//

#include "hotkey_manager.h"

HotkeyManager::HotkeyManager(const EventStreamWriter &event_stream_writer, void *const event_stream) : event_stream_writer(event_stream_writer), event_stream_buffer(event_stream) {}

std::map<uint32_t, GameBridgeEvents> HotkeyManager::PollHotkeys() {
    // Not yet implemented, relies on IHotKeys interface.
    // Todo: Implement mechanism that takes key bits from the IHotkeys interface for every registered hotkey and returns which ones are being pressed based on the key_combo map.
    return {};
}

void HotkeyManager::AddHotkey(uint32_t key_bits, GameBridgeEvents event_type) {
    key_combo.insert(std::map<uint32_t, GameBridgeEvents>::value_type (0, event_type));
}

void HotkeyManager::RemoveHotkey(uint32_t key_bits) {
    key_combo.erase(key_bits);
}

void HotkeyManager::SendHotkeyEvents() {
    std::map<uint32_t, GameBridgeEvents> pressed_keys = PollHotkeys();
    for(std::map<uint32_t, GameBridgeEvents>::iterator it = pressed_keys.begin(); it != pressed_keys.end(); it++){
        // Todo: Assuming the uint32_t is always 4 bytes, we use that as the size of the payload. (This is probably wrong?)
        event_stream_writer.SubmitEvent(it->second, 4, reinterpret_cast<void *>((uintptr_t) it->first));
    }
}

void *HotkeyManager::GetEventBuffer() {
    return event_stream_buffer;
}

GameBridgeManagerType HotkeyManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_HOTKEY;
}

