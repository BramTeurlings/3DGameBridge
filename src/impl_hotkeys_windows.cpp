//
// Created by Bram on 22/06/2023.
//

#include "hotkeys_interface.h"

#include <iostream>
#include <vector>
#include <Windows.h>
#include <map>

class WindowsHotkeyImplementation : private IHotkeys {
    public:
    std::vector<std::map<uint32_t, bool>> CheckHotkeys(std::vector<uint32_t> keyCodes) override {
        // Clear list of hotkey states.
        hotkey_states.clear();

        std::vector<std::map<uint32_t, bool>> keyStates;

        // Loop through the key codes
        for (size_t i = 0; i < keyCodes.size(); i++)
        {
            // Check the state of each key code
            auto keyCode = static_cast<int>(keyCodes[i]);

            // Todo: Microsoft sample checks against 0x01 instead, we need to check if this works. We're supposed to check the most significant bit to get the read state.
            // By performing a bitwise AND operation between the GetAsyncKeyState result and 0x8000.
            // We extract the most significant bit, which corresponds to the key state.
            // If the result is non-zero (bit 15 is set), it means the key is pressed.
            // If the result is zero (bit 15 is not set), it means the key is released.
            bool isKeyPressed = GetAsyncKeyState(keyCode) & 0x8000;

            // Update the key state in the vector
            std::map<uint32_t, bool> keyState;
            keyState[keyCode] = isKeyPressed;

            // Add the map to the vector
            keyStates.push_back(keyState);
        }

        // Todo: Check if this is using the copy constructor, because I want to make a copy here.
        // Set the list of pressed_keys.
        hotkey_states = keyStates;

        // Returns the key states
        return keyStates;
    }

    std::vector<uint32_t> GetKeysPressed() override {
        // Clear list of pressed keys.
        pressed_keys.clear();

        std::vector<uint32_t> keysPressed;

        // Loop through the possible key codes
        for (uint32_t keyCode = 0; keyCode < 256; keyCode++)
        {
            // Todo: Microsoft sample checks against 0x01 instead, we need to check if this works. We're supposed to check the most significant bit to get the read state.
            // By performing a bitwise AND operation between the GetAsyncKeyState result and 0x8000.
            // We extract the most significant bit, which corresponds to the key state.
            // If the result is non-zero (bit 15 is set), it means the key is pressed.
            // If the result is zero (bit 15 is not set), it means the key is released.
            bool isKeyPressed = GetAsyncKeyState(static_cast<int>(keyCode)) & 0x8000;

            // Add the key code to the vector if it's pressed
            if (isKeyPressed)
            {
                keysPressed.push_back(keyCode);
            }
        }

        // Todo: Check if this is using the copy constructor, because I want to make a copy here.
        // Set the list of pressed_keys.
        pressed_keys = keysPressed;

        // Return the vector of pressed keys
        return keysPressed;
    }
};
