//
// Created by Bram on 08/06/2023.
//

#pragma once

#include "game_bridge_structs.h"
#include <cstdint>
#include <vector>
#include <map>

using CombinedStrokes = uint32_t;
using SeparateStroke = uint8_t;

class IHotkeys {
public:
    // Hotkeys are comprised of 8 bits per keystroke. We support any hotkey combination as long as it is no more than 4 keys.
    // Hotkeys are split into each individual keystroke by use of a union with uint8_t strokes[4].
    union CombinedHotkeyStrokes {
        CombinedStrokes combinedNumber;
        SeparateStroke separatedStrokes[4];
    };

    // Defines a custom comparator for the union type
    struct UnionComparator {
        bool operator()(const CombinedHotkeyStrokes& left, const CombinedHotkeyStrokes& right) const {
            return left.combinedNumber < right.combinedNumber;
        }
    };

    // Saves all pressed key on the keyboard since last check.
    std::vector<CombinedStrokes> pressed_keys;

    // Saves the activation state of every registered hotkey since last check.
    std::map<CombinedHotkeyStrokes, bool, UnionComparator> hotkey_states;

    IHotkeys() = default;
    virtual ~IHotkeys() = default;

    // Checks for every given key inside the hotkeys if it is pressed or not and stores the result inside the hotkey_states object.
    // Resets all keys in the hotkey_states vector at the start of the operation.
    virtual std::map<CombinedHotkeyStrokes, bool, UnionComparator> CheckHotkeys(std::vector<CombinedHotkeyStrokes> key_codes) = 0;

    // Returns all the keys that are currently in the pressed state regardless of if hotkeys are assigned to them.
    // Resets all keys in the pressed_keys vector at the start of the operation.
    //
    // Note: Costs quite some processing time. ~4ms
    virtual std::vector<uint32_t> GetKeysPressed() = 0;
};