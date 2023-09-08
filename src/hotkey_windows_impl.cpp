//
// Created by Bram on 22/06/2023.
//

#include "hotkey_interface.h"

#include <iostream>
#include <vector>
#include <Windows.h>
#include <map>
#include <algorithm>

class WindowsHotkeyImplementation : public IHotkeys {
    public:
    WindowsHotkeyImplementation() = default;
    ~WindowsHotkeyImplementation() override = default;

    // Todo: Figure out a good return type here. This isn't working.
    std::map<IHotkeys::HotkeyCombination, bool, UnionComparator>
    CheckHotkeys(std::vector<IHotkeys::HotkeyCombination> keyCodes) override {
        // Prepare the return vector
        std::map<IHotkeys::HotkeyCombination, bool, UnionComparator> result{};

        // Clear list of hotkey states.
        hotkey_states.clear();

        // List of keycodes with their status.
        std::map<uint8_t, bool> keyStates;

        // Loop through the hotkeys
        for (auto it = keyCodes.begin(); it != keyCodes.end(); it++)
        {
            // Loop through the key codes
            for (int i = 0; i < 4; i++) {
                // Check the state of each key code
                auto keyCode = it->separatedStrokes[i];

                // Skip if key has already been checked.
                if (keyStates.count(keyCode)){
                    continue;
                }

                // Todo: Microsoft sample checks against 0x01 instead, we need to check if this works. We're supposed to check the most significant bit to get the read state.
                // By performing a bitwise AND operation between the GetAsyncKeyState result and 0x8000.
                // We extract the most significant bit, which corresponds to the key state.
                // If the result is non-zero (bit 15 is set), it means the key is pressed.
                // If the result is zero (bit 15 is not set), it means the key is released.
                bool isKeyPressed = GetAsyncKeyState((int)keyCode) & 0x8000;

                // Add key status to the map.
                keyStates.insert({keyCode, isKeyPressed});
            }

            // Combine all keycodes that correspond to the current hotkey into one uint32_t, substitute 0 when the key is not pressed.
            HotkeyCombination currentHotkeyStatus;
            currentHotkeyStatus.separatedStrokes[0] = (keyStates.find(it->separatedStrokes[0])->second) ? keyStates.find(it->separatedStrokes[0])->first : 0x0;
            currentHotkeyStatus.separatedStrokes[1] = (keyStates.find(it->separatedStrokes[1])->second) ? keyStates.find(it->separatedStrokes[1])->first : 0x0;
            currentHotkeyStatus.separatedStrokes[2] = (keyStates.find(it->separatedStrokes[2])->second) ? keyStates.find(it->separatedStrokes[2])->first : 0x0;
            currentHotkeyStatus.separatedStrokes[3] = (keyStates.find(it->separatedStrokes[3])->second) ? keyStates.find(it->separatedStrokes[3])->first : 0x0;

            // Sort all the stokes to be consistent with the registered keystokes.
            std::sort(&currentHotkeyStatus.separatedStrokes[0],&currentHotkeyStatus.separatedStrokes[4]);

            if((currentHotkeyStatus.combinedNumber & it->combinedNumber) == it->combinedNumber) {
                // The hotkey is pressed.
                result[it.operator*()] = true;
            } else {
                // The hotkey is not pressed.
                result[it.operator*()] = false;
            }

        }

        // Todo: Check if this is using the copy constructor, because I want to make a copy here.
        // Set the list of pressed_keys.
        hotkey_states = result;

        // Returns the key states
        return result;
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
