//
// Created by Bram on 07/06/2023.
//
#pragma once
#include "gb_structs.h"

class IGameBridgeManager {
public:
    GameBridgeManagerType game_bridge_manager_type = GameBridgeManagerType::SRGB_MANAGER_HOTKEY;

    IGameBridgeManager() = default;
    virtual ~IGameBridgeManager() = default;

    virtual GameBridgeManagerType GetEventManagerType() = 0;
};
