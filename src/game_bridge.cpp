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
