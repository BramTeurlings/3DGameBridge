#pragma once

#ifdef GAME_BRIDGE_API_EXPORTS
#define GAME_BRIDGE_API __declspec(dllexport)
#else
#define GAME_BRIDGE_API __declspec(dllimport)
#endif

namespace srreshade {
    extern "C" GAME_BRIDGE_API void init_api();
}
