#pragma once

// TODO Move all the export definitions in the project to an common header file for internal use only (if that works)
#include <cstddef>
#include <cstdint>

#ifdef GAME_BRIDGE_API_EXPORTS
#define GAME_BRIDGE_API __declspec(dllexport)
#else
#define GAME_BRIDGE_API __declspec(dllimport)
#endif

typedef uint32_t GB_EVENT;

// Managers
enum GAME_BRIDGE_API GameBridgeManagerType {
    GB_MANAGER_HOTKEY,
    GB_MANAGER_EVENTS,
    GB_MANAGER_WEAVER_DX11,
    GB_MANAGER_WEAVER_DX12,
    GB_MANAGER_PLATFORM
};

// Initialization
enum GAME_BRIDGE_API GameBridgeEventManagerInitialize {
    GB_PROCESS_EVENTS_AT_THE_END_OF_CURRENT_FRAME,
    GB_PROCESS_EVENTS_ON_THE_NEXT_FRAME
};

enum GAME_BRIDGE_API GameBridgeHotkeyManagerInitializeFlags {
    GB_HOTKEY_MANAGER_NO_FLAGS
};

// Event manager types
enum GAME_BRIDGE_API GameBridgeEventManagerType {
    GB_EVENT_MANAGER_TYPE_HOTKEY,
    GB_EVENT_MANAGER_TYPE_PLATFORM,
    GB_EVENT_MANAGER_TYPE_WEAVER
};

// Messages
// Reserve 0 as the NULL EVENT
constexpr size_t GB_EVENT_NULL = 0;

enum GAME_BRIDGE_API GameBridgeHotKeyEvent {
    GB_EVENT_HOTKEY_TOGGLE_LENS = 1
};

enum GAME_BRIDGE_API GameBridgePlatformEvent {
    GB_EVENT_PLATFORM_CONTEXT_INVALIDATED = 1
};

enum GAME_BRIDGE_API GameBridgeWeaverEvent {
    GB_EVENT_WEAVER_WEAVING_ENABLED = 1
};

// SR weaver flags
enum GAME_BRIDGE_API GameBridgeWeaverFlags {
    GB_ENABLE_DEBUG_OVERLAY_OR_WHATEVER,
    GB_MANUAL_SET_CAMERA_LATENCY_VALUE,
    GB_MANUAL_SET_CAMERA_LATENCY_IN_FRAMES__VALUE
};
