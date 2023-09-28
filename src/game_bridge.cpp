#include "game_bridge.h"
#include <iostream>

namespace game_bridge {
#ifdef GAME_BRIDGE_API_EXPORTS
    GAME_BRIDGE_API void init_api() {
        //Not yet implemented.
    }
#else
    GAME_BRIDGE_API void init_api();
#endif
}

GameBridge::GameBridge(EventManager eventManager) : event_manager(std::move(eventManager)) {}

bool GameBridge::RegisterManager(IGameBridgeManager *manager) {
    //Todo: Implement this.
    return false;
}

EventManager GameBridge::GetEventManager() {
    return event_manager;
}

bool GameBridge::DeregisterManager() {
    //Todo: Implement this.
    return false;
}
