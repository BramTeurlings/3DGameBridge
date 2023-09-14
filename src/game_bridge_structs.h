#pragma once

// TODO Move all the export definitions in the project to an common header file for internal use only (if that works)
#ifdef GAME_BRIDGE_API_EXPORTS
#define GAME_BRIDGE_API __declspec(dllexport)
#else
#define GAME_BRIDGE_API __declspec(dllimport)
#endif

// TODO More consisten enum naming conventions
// These name will are not clear and having no enum classes here anymore makes that worse

typedef uint32_t GB_EVENT;

// The manager type
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

// Event types for a manager
enum GAME_BRIDGE_API EventManagerType {
    SRGB_EVENT_MANAGER_TYPE_HOTKEY,
    SRGB_EVENT_MANAGER_TYPE_PLATFORM,
    SRGB_EVENT_MANAGER_TYPE_WEAVER
};

// Messages
// 0 Should be reserved for null event
enum GAME_BRIDGE_API HotKeyEvent {
    GB_EVENT_HOTKEY_TOGGLE_LENS = 1
};

enum GAME_BRIDGE_API PlatformEvent {
    SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED = 1
};

enum GAME_BRIDGE_API WeaverEvent {
    SRGB_EVENT_WEAVER_WEAVING_ENABLED = 1
};

enum GAME_BRIDGE_API WeaverFlags {
    SRGB_ENABLE_DEBUG_OVERLAY_OR_WHATEVER,
    SRGB_MANUAL_SET_CAMERA_LATENCY_VALUE,
    SRGB_MANUAL_SET_CAMERA_LATENCY_IN_FRAMES__VALUE
};

// Utility
struct ClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

