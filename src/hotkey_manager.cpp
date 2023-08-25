//
// Created by Bram on 15/06/2023.
//

#include "hotkey_manager.h"
#include "impl_hotkeys_windows.cpp"

HotkeyManager::HotkeyManager(HotkeyManagerInitialize initialize) : implementation(initialize.implementation) {
    void* event_stream;
    //event_stream_writer = initialize.game_bridge.GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_HOTKEY, &event_stream);
    //event_stream_buffer = event_stream;
}

void HotkeyManager::PollHotkeys() {
    // Polls the hotkeys using the current implementation of the IHotkeys interface. (Statically defined as Windows at present).

    //Windows Implementation:
    WindowsHotkeyImplementation hotkey_manager;

    std::vector<uint32_t> hotkeys;
    for(uint32_t i = 0; i < key_combo.size();) {
        hotkeys.push_back(key_combo[i]);
    }

    //Let the IHotkeys interface check the hotkeys, they can be retrieved from its "hotkey_states" member.
    std::vector<std::map<uint32_t, bool>> checkedHotkeys = hotkey_manager.CheckHotkeys(hotkeys);
}

void HotkeyManager::AddHotkey(uint32_t key_bits, GameBridgeEvents event_type) {
    key_combo.insert(std::map<uint32_t, GameBridgeEvents>::value_type (0, event_type));
}

void HotkeyManager::RemoveHotkey(uint32_t key_bits) {
    key_combo.erase(key_bits);
}

void HotkeyManager::SendHotkeyEvents() {
    for(int i = 0; i < implementation->hotkey_states.size(); i++){
        auto it = implementation->hotkey_states[i].begin();
        if(it->second) {
            // Todo: Assuming the uint32_t is always 4 bytes, we use that as the size of the payload. (This is probably wrong?)
            event_stream_writer->SubmitEvent(it->second, 4, reinterpret_cast<void *>((uintptr_t) it->first));
        }
        it++;
    }
}

void *HotkeyManager::GetEventBuffer() {
    return event_stream_buffer;
}

GameBridgeManagerType HotkeyManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_HOTKEY;
}

