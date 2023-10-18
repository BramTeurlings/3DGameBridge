//
// Created by Bram on 15/06/2023.
//

#include "hotkey_windows_impl.h"
#include "hotkey_manager.h"
#include <algorithm>

HotkeyManager::HotkeyManager(HotkeyManagerInitialize initialize) : implementation(initialize.implementation) {
    // Todo: Wait for event manager implementation.
    //void* event_stream;
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
    std::vector<HotkeyCombination> hotkeys;
    for(auto it = registered_hotkeys.begin(); it != registered_hotkeys.end(); it++) {
        hotkeys.push_back(it->hotkeyCombination);
    }

    //Let the IHotkeys interface check the hotkeys, they can be retrieved later from its "hotkey_states" member.
    if(std::map<HotkeyCombination, bool, IHotkeys::UnionComparator> checkedHotkeys = this->implementation->CheckHotkeys(hotkeys); checkedHotkeys.empty()){
        // No hotkeys were registered
        return false;
    }

    return true;
}

void HotkeyManager::AddHotkey(HotKeyEvent event_type, uint8_t first_keystroke, uint8_t second_keystroke, uint8_t third_keystroke, uint8_t fourth_keystroke) {
    // Start by combining all shortcuts into the union.
    HotkeyCombination received_strokes{};
    received_strokes.separatedStrokes[0] = first_keystroke;
    received_strokes.separatedStrokes[1] = second_keystroke;
    received_strokes.separatedStrokes[2] = third_keystroke;
    received_strokes.separatedStrokes[3] = fourth_keystroke;

    // Sort all keystrokes
    std::sort(&received_strokes.separatedStrokes[0],&received_strokes.separatedStrokes[4]);

    registered_hotkeys.push_back({ received_strokes, event_type });
}



void HotkeyManager::RemoveHotkey(uint32_t combined_number, HotKeyEvent event_type) {
    for(int i = 0; i < registered_hotkeys.size(); i++) {
        if(registered_hotkeys[i].hotkeyCombination.combinedNumber == combined_number && registered_hotkeys[i].hotkeyEvent == event_type) {
            // Found matching hotkey, time to remove it!
            registered_hotkeys.erase(registered_hotkeys.begin() + i);
        }
    }
}

void HotkeyManager::SendHotkeyEvents() {
    std::map<HotkeyCombination, bool>::iterator it;

    for (int i = 0; i < registered_hotkeys.size(); i++) {
        if(implementation->hotkey_states[registered_hotkeys[i].hotkeyCombination]) {
            // Found pressed hotkey, send event.
            event_stream_writer->SubmitEvent(registered_hotkeys[i].hotkeyEvent, 0,
                                             nullptr);
        }
    }
}

GameBridgeManagerType HotkeyManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_HOTKEY;
}
