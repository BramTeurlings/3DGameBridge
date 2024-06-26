#pragma once

#include <utility>
#include <vector>
#include "game_bridge_manager_interface.h"
#include "event_manager.h"
#include "game_bridge_structs.h"

#ifdef GAME_BRIDGE_API_EXPORTS
#define GAME_BRIDGE_API __declspec(dllexport)
#else
#define GAME_BRIDGE_API __declspec(dllimport)
#endif

namespace game_bridge {
    extern "C" GAME_BRIDGE_API void init_api();
}

class GAME_BRIDGE_API GameBridge {
    std::vector<IGameBridgeManager*> game_bridge_managers;
    EventManager event_manager;

public:
    explicit GameBridge(EventManager eventManager);

    // Registers the provided manager on the event_manager and saves the IGameBridgeManager inside the game_bridge_manager list.
    bool RegisterManager(IGameBridgeManager* manager);

    EventManager GetEventManager();

    // Unregisters the current event_manager.
    bool DeregisterManager();
};
