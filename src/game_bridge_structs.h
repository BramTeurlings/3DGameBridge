#pragma once

// TODO Move all the export definitions in the project to an common header file for internal use only (if that works)
#include <cstddef>
#include <cstdint>

#ifdef GAME_BRIDGE_API_EXPORTS
#define GAME_BRIDGE_API __declspec(dllexport)
#else
#define GAME_BRIDGE_API __declspec(dllimport)
#endif

// TODO More consistent enum naming conventions
// These names are not clear and having no enum classes here anymore makes that worse

typedef uint32_t GB_EVENT;

// Managers
enum GAME_BRIDGE_API GameBridgeManagerType {
    SRGB_MANAGER_HOTKEY,
    SRGB_MANAGER_EVENTS,
    SRGB_MANAGER_WEAVER_DX11,
    SRGB_MANAGER_WEAVER_DX12,
    SRGB_MANAGER_PLATFORM
};

// Initialization
enum GAME_BRIDGE_API EventManagerInitialize {
    PROCESS_EVENTS_AT_THE_END_OF_CURRENT_FRAME,
    PROCESS_EVENTS_ON_THE_NEXT_FRAME
};

enum GAME_BRIDGE_API HotkeyManagerInitializeFlags {
    SRGB_HOTKEY_MANAGER_NO_FLAGS
};

// Event manager types
enum GAME_BRIDGE_API EventManagerType {
    SRGB_EVENT_MANAGER_TYPE_HOTKEY,
    SRGB_EVENT_MANAGER_TYPE_PLATFORM,
    SRGB_EVENT_MANAGER_TYPE_WEAVER
};

// Messages
// Reserve 0 as the NULL EVENT
constexpr size_t GB_EVENT_NULL = 0;

enum GAME_BRIDGE_API HotKeyEvent {
    GB_EVENT_HOTKEY_TOGGLE_LENS = 1
};

enum GAME_BRIDGE_API PlatformEvent {
    SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED = 1
};

enum GAME_BRIDGE_API WeaverEvent {
    SRGB_EVENT_WEAVER_WEAVING_ENABLED = 1
};

// SR weaver flags
enum GAME_BRIDGE_API WeaverFlags {
    SRGB_ENABLE_DEBUG_OVERLAY_OR_WHATEVER,
    SRGB_MANUAL_SET_CAMERA_LATENCY_VALUE,
    SRGB_MANUAL_SET_CAMERA_LATENCY_IN_FRAMES__VALUE
};
