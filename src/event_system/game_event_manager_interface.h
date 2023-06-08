//
// Created by Bram on 07/06/2023.
//
#pragma once

enum class GameBridgeManagerType {
    SRGB_MANAGER_HOTKEY,
    SRGB_MANAGER_EVENTS,
    SRGB_MANAGER_WEAVER_DX11,
    SRGB_MANAGER_WEAVER_DX12,
    SRGB_MANAGER_PLATFORM
};

class IGameBridgeManager {
public:
    GameBridgeManagerType game_bridge_manager_type = GameBridgeManagerType::SRGB_MANAGER_HOTKEY;

    IGameBridgeManager() = default;
    virtual ~IGameBridgeManager() = default;

    virtual GameBridgeManagerType GetEventManagerType() = 0;
};
