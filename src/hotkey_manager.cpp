//
// Created by Bram on 15/06/2023.
//

#include "hotkey_windows_impl.h"
#include "hotkey_manager.h"
#include <algorithm>

HotkeyManager::HotkeyManager(HotkeyManagerInitialize initialize) : implementation(initialize.implementation) {
    // Todo: Wait for event manager implementation.
    event_stream_writer = initialize.game_bridge.GetEventManager().CreateEventStream(EventManagerType::SRGB_EVENT_MANAGER_TYPE_HOTKEY, 100, 0);
}

// Polls the hotkeys using the current implementation of the IHotkeys interface. (Currently defaults to the Windows Implementation).
bool HotkeyManager::PollHotkeys() {
    //Check if hotkey implementation is initialized
    if (this->implementation == nullptr){
        //Hotkey Implementation is not initialized.
        return false;
    }

    // Adds all registered hotkeys to the list of hotkeys.
    std::vector<IHotkeys::CombinedHotkeyStrokes> hotkeys;
    for(auto it = registered_hotkeys.begin(); it != registered_hotkeys.end(); it++) {
        hotkeys.push_back(it->hotkey_combination);
    }

    //Let the IHotkeys interface check the hotkeys, they can be retrieved later from its "hotkey_states" member.
    if(this->implementation->CheckHotkeys(hotkeys).empty()){
        // No hotkeys were registered
        return false;
    }

    return true;
}

void HotkeyManager::AddHotkey(HotKeyEvent event_type, uint8_t first_keystroke, uint8_t second_keystroke, uint8_t third_keystroke, uint8_t fourth_keystroke) {
    // Start by combining all shortcuts into the union.
    IHotkeys::CombinedHotkeyStrokes received_strokes{};
    received_strokes.separatedStrokes[0] = first_keystroke;
    received_strokes.separatedStrokes[1] = second_keystroke;
    received_strokes.separatedStrokes[2] = third_keystroke;
    received_strokes.separatedStrokes[3] = fourth_keystroke;

    // Sort all keystrokes
    std::sort(&received_strokes.separatedStrokes[0],&received_strokes.separatedStrokes[4]);

    HotkeyContainer tempContainer;
    tempContainer.hotkey_combination = received_strokes;
    tempContainer.hotkey_event = event_type;
    registered_hotkeys.push_back(tempContainer);
}



void HotkeyManager::RemoveHotkey(CombinedStrokes combined_number, HotKeyEvent event_type) {
    for (auto it = registered_hotkeys.begin(); it != registered_hotkeys.end(); ++it) {
        if (it->hotkey_combination.combinedNumber == combined_number && it->hotkey_event == event_type) {
            // Found matching hotkey, time to remove it!
            registered_hotkeys.erase(it);
            return;
        }
    }
}

bool HotkeyManager::SendHotkeyEvents() {
    for (auto it = registered_hotkeys.begin(); it != registered_hotkeys.end(); ++it) {
        if (implementation->hotkey_states[it->hotkey_combination]) {
            // Found pressed hotkey, send event.
            return event_stream_writer->SubmitEvent(it->hotkey_event, 0, nullptr);
        }
    }
}

GameBridgeManagerType HotkeyManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_HOTKEY;
}
