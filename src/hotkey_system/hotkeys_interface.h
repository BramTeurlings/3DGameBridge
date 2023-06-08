//
// Created by Bram on 08/06/2023.
//

#include <cstdint>
#include <vector>

#ifndef INC_3DGAMEBRIDGE_HOTKEYS_INTERFACE_H
#define INC_3DGAMEBRIDGE_HOTKEYS_INTERFACE_H

#endif //INC_3DGAMEBRIDGE_HOTKEYS_INTERFACE_H
#pragma once

class IHotkeys {
    // Todo: Do we opt to store pressed keys here or have them stored in the implementation of this interface.
    std::vector<uint32_t> pressedKeys;

public:
    IHotkeys() = default;
    virtual ~IHotkeys() = default;


    // Todo: I'm not sure how this will work, did we make a mistake in the class diagram?
    // Checks for every given key if it is pressed or not and stores the result somewhere.
    virtual void CheckHotkeys(std::vector<uint32_t>) = 0;

    // Returns all the keys that are currently in the pressed state regardless of if hotkeys are assigned to them.
    // Resets the list of currently pressed keys afterward.
    virtual std::vector<uint32_t> GetKeysPressed() = 0;
};