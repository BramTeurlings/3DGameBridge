//
// Created by Bram on 15/06/2023.
//

#include <algorithm>
#include "hotkey_manager.h"
#include "hotkey_windows_impl.cpp"

HotkeyManager::HotkeyManager(HotkeyManagerInitialize initialize) : implementation(initialize.implementation) {
void* event_stream;
    //event_stream_writer = initialize.game_bridge.GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_HOTKEY, &event_stream);
    //event_stream_buffer = event_stream;
}

bool HotkeyManager::PollHotkeys() {
    // Polls the hotkeys using the current implementation of the IHotkeys interface. (Statically defined as Windows at present).

    //Check if hotkey implementation is initialized
    if (this->implementation == nullptr){
        //Hotkey Implementation is not initialized.
        return false;
    }

    // Adds all registered hotkeys to the list of hotkeys.
    std::vector<IHotkeys::HotkeyCombination> hotkeys;
    for(auto it = registered_hotkeys.begin(); it != registered_hotkeys.end(); it++) {
        hotkeys.push_back(it->hotkeyCombination);
    }

    // Todo: Maybe do something useful with the checkedHotkeys object here.
    //Let the IHotkeys interface check the hotkeys, they can be retrieved from its "hotkey_states" member.
    std::map<IHotkeys::HotkeyCombination, bool, IHotkeys::UnionComparator> checkedHotkeys = this->implementation->CheckHotkeys(hotkeys);

    return true;
}

void HotkeyManager::AddHotkey(HotKeyEvent event_type, uint8_t first_keystroke, uint8_t second_keystroke, uint8_t third_keystroke, uint8_t fourth_keystroke) {
//    union a{
//        uint32_t num;
//        uint8_t strokes[4];
//    };
//
//    uint32_t num;
//    num = 0b11111111;
//    // 0b11111111
//    num << 8;
//    // 0b1111111100000000
//
//    uint32_t num2 = 0b1111111100000000;
//
//    uint32_t num3 = num +num2; // 0b1111111111111111;
//    uint32_t num4 = num3 >> 8;
//
//    uint32_t some_key = 0x1111000011111111;
//
//    // Compare the completed sorted uint32_t of the reportedly pressed keys form the OS against the registered keys.
//    (num2 & some_key) == some_key;
//
//    a b{
//            0b11111111000000000000000000000000
//    };
//    b.strokes[1];
//
//    // Sort both the registered hotkeys and the combined keystrokes from the OS.
//    std::sort(&b.strokes[0],&b.strokes[4]);

    // Start by combining all shortcuts into the union.
    IHotkeys::HotkeyCombination received_strokes{};
    received_strokes.separatedStrokes[0] = first_keystroke;
    received_strokes.separatedStrokes[1] = second_keystroke;
    received_strokes.separatedStrokes[2] = third_keystroke;
    received_strokes.separatedStrokes[3] = fourth_keystroke;

    // Sort all keystrokes
    std::sort(&received_strokes.separatedStrokes[0],&received_strokes.separatedStrokes[4]);

    registered_hotkeys.push_back({ received_strokes, event_type });
}



void HotkeyManager::RemoveHotkey(uint32_t key_bits) {
//    key_combo.erase(key_bits);
}

void HotkeyManager::SendHotkeyEvents() {
//    for(int i = 0; i < implementation->hotkey_states.size(); i++){
//        auto it = implementation->hotkey_states[i].begin();
//        if(it->second) {
//            // Todo: Assuming the uint32_t is always 4 bytes, we use that as the size of the payload. (This is probably wrong?)
//            event_stream_writer->SubmitEvent(it->second, 4, reinterpret_cast<void *>((uintptr_t) it->first));
//        }
//        it++;
//    }
}

void *HotkeyManager::GetEventBuffer() {
    return event_stream_buffer;
}

GameBridgeManagerType HotkeyManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_HOTKEY;
}
