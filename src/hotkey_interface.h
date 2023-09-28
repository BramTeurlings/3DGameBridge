//
// Created by Bram on 08/06/2023.
//

#include <cstdint>
#include <vector>
#include <map>

#ifndef INC_3DGAMEBRIDGE_HOTKEYS_INTERFACE_H
#define INC_3DGAMEBRIDGE_HOTKEYS_INTERFACE_H

#endif //INC_3DGAMEBRIDGE_HOTKEYS_INTERFACE_H
#pragma once

class IHotkeys {
public:
    // Todo: Do we opt to store pressed keys here or have them stored in the implementation of this interface.
    std::vector<uint32_t> pressed_keys;
    std::vector<std::map<uint32_t, bool>> hotkey_states;

    IHotkeys() = default;
    virtual ~IHotkeys() = default;


    // Todo: For the two methods below, do we want them to return a list of pressed strings right away or simply save the keystates inside this interface's fields?

    // Checks for every given key if it is pressed or not and stores the result somewhere.
    // Resets all keys in the hotkey_states vector at the start of the operation.
    virtual std::vector<std::map<uint32_t, bool>> CheckHotkeys(std::vector<uint32_t> keyCodes) = 0;

    // Returns all the keys that are currently in the pressed state regardless of if hotkeys are assigned to them.
    // Resets all keys in the pressed_keys vector at the start of the operation.
    virtual std::vector<uint32_t> GetKeysPressed() = 0;
};