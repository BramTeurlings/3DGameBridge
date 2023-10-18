//
// Created by Bram on 14/09/2023.
//

#pragma once

#include "hotkey_interface.h"

#ifdef GAME_BRIDGE_API_EXPORTS
#define GAME_BRIDGE_API __declspec(dllexport)
#else
#define GAME_BRIDGE_API __declspec(dllimport)
#endif

class GAME_BRIDGE_API WindowsHotkeyImplementation : public IHotkeys {
public:
    WindowsHotkeyImplementation() = default;

    ~WindowsHotkeyImplementation() override = default;

    std::map<CombinedHotkeyStrokes, bool, UnionComparator> CheckHotkeys(std::vector<CombinedHotkeyStrokes> key_codes) override;

    std::vector<uint32_t> GetKeysPressed() override;
};
