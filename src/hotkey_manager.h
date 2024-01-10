//
// Created by Bram on 08/06/2023.
//

#pragma once

#include "hotkey_interface.h"
#include "event_manager.h"
#include "game_bridge.h"
#include "game_bridge_structs.h"
#include <map>

struct GAME_BRIDGE_API HotkeyManagerInitialize {
    std::shared_ptr<IHotkeys> implementation;
    GameBridgeHotkeyManagerInitialize flags;
    GameBridge game_bridge;
};

struct HotkeyContainer {
    IHotkeys::CombinedHotkeyStrokes hotkey_combination;
    GameBridgeHotKeyEvent hotkey_event;
};

class GAME_BRIDGE_API HotkeyManager : private IGameBridgeManager {
public:
    std::vector<HotkeyContainer> registered_hotkeys;
    std::shared_ptr<EventStreamWriter> event_stream_writer;
    std::shared_ptr<IHotkeys> implementation;

    HotkeyManager() = default;

    explicit HotkeyManager(HotkeyManagerInitialize initialize);

    // Checks if the specified hotkeys are being pressed or not.
    // Returns true if one or more hotkey(s) were successfully checked.
    // Returns false if the operation was not successful, usually due to an uninitialized hotkey manager implementation or if no hotkeys were registered.
    bool PollHotkeys();

    // Adds the specified hotkey to the list of registered hotkeys.
    // key_bits supports a total of 4 keystrokes to be added, one keystroke is 8 bits of binary data.
    //
    // The windows implementation maps each keystroke as 8 bits of binary which corresponds to the keycodes on Microsoft's website.
    //
    // event_type: The type of event that should be triggered and sent upon pressing the hotkey combination.
    // first_keystroke: Mandatory single keystroke that will trigger the hotkey when it is the only keystroke that was added.
    // second_keystroke: Additional keystroke, all non-zero keystrokes must be pressed for the hotkey to trigger.
    // third_keystroke: Additional keystroke, all non-zero keystrokes must be pressed for the hotkey to trigger.
    // fourth_keystroke: Additional keystroke, all non-zero keystrokes must be pressed for the hotkey to trigger.
    void AddHotkey(GameBridgeHotKeyEvent event_type, uint8_t first_keystroke, uint8_t second_keystroke = 0, uint8_t third_keystroke = 0, uint8_t fourth_keystroke = 0);

    // Removes the specified hotkey from the list of registered hotkeys.
    //
    // combined_number: The uint32_t representing the key codes requires to trigger the hotkey.
    // event_type: The enum class representing the type of event that should trigger when pressing the hotkey.
    void RemoveHotkey(uint32_t combined_number, GameBridgeHotKeyEvent event_type);

    // Uses the event_stream_writer to send all generated events to the EventManager depending on what keys are pressed.
    // This will return false when the send operation failed, a failure will not invalidate events sent earlier.
    bool SendHotkeyEvents();

    GameBridgeManagerType GetEventManagerType() override;
};


