//
// Created by Bram on 07/06/2023.
//

#pragma once

#include "game_bridge_structs.h"

class GAME_BRIDGE_API IGameBridgeManager {
public:
    GameBridgeManagerType game_bridge_manager_type;

    IGameBridgeManager() = default;
    virtual ~IGameBridgeManager() = default;

    virtual GameBridgeManagerType GetEventManagerType() = 0;
};
