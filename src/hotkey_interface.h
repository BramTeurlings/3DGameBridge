//
// Created by Bram on 08/06/2023.
//

#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "game_bridge_structs.h"

class IHotkeys {
public:
    // Hotkeys are comprised of 8 bits per keystroke. We support any hotkey combination as long as it is no more than 4 keys.
    // Hotkeys are split into each individual keystroke by use of a union with uint8_t strokes[4].
    union HotkeyCombination {
        uint32_t combinedNumber;
        uint8_t separatedStrokes[4];

        HotkeyCombination() {};
        ~HotkeyCombination() {};
    };

    // Defines a custom comparator for the union type
    struct UnionComparator {
        bool operator()(const HotkeyCombination& left, const HotkeyCombination& right) const {
            return left.combinedNumber < right.combinedNumber;
        }
    };

    struct HotkeyContainer {
        HotkeyCombination hotkeyCombination;
        HotKeyEvent hotkeyEvent;
    };

    // Todo: Do we opt to store pressed keys here or have them stored in the implementation of this interface. Also, shouldn't we just use hotkey_states?
    std::vector<uint32_t> pressed_keys;

    // Todo: Should this not be private?
    // Saves the state of every registered hotkey
    std::map<IHotkeys::HotkeyCombination, bool, UnionComparator> hotkey_states;

    IHotkeys() = default;
    virtual ~IHotkeys() = default;


    // Todo: For the two methods below, do we want them to return a list of pressed strings right away or simply save the keystates inside this interface's fields?

    // Checks for every given key if it is pressed or not and stores the result somewhere.
    // Resets all keys in the hotkey_states vector at the start of the operation.
    virtual std::map<IHotkeys::HotkeyCombination, bool, UnionComparator> CheckHotkeys(std::vector<IHotkeys::HotkeyCombination> keyCodes) = 0;

    // Returns all the keys that are currently in the pressed state regardless of if hotkeys are assigned to them.
    // Resets all keys in the pressed_keys vector at the start of the operation.
    virtual std::vector<uint32_t> GetKeysPressed() = 0;
};