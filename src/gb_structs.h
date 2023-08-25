#pragma once

typedef uint32_t GameBridgeEvents;

// Managers
enum class GameBridgeManagerType {
    SRGB_MANAGER_HOTKEY,
    SRGB_MANAGER_EVENTS,
    SRGB_MANAGER_WEAVER_DX11,
    SRGB_MANAGER_WEAVER_DX12,
    SRGB_MANAGER_PLATFORM
};

// Initialization
enum class EventManagerInitialize {
    PROCESS_EVENTS_AT_THE_END_OF_CURRENT_FRAME,
    PROCESS_EVENTS_ON_THE_NEXT_FRAME
};

// Event types
enum class EventManagerType {
    SRGB_EVENT_MANAGER_TYPE_HOTKEY,
    SRGB_EVENT_MANAGER_TYPE_PLATFORM,
    SRGB_EVENT_MANAGER_TYPE_WEAVER
};

// Messages
// 0 Should be reserved for null event
enum class HotKeyEvent : GameBridgeEvents {
    GB_EVENT_HOTKEY_TURN_ON_LENS = 1,
    GB_EVENT_HOTKEY_TURN_OFF_LENS
};

enum class  PlatformEvent : GameBridgeEvents {
    SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED = 1
};

enum class WeaverEvent : GameBridgeEvents {
    SRGB_EVENT_WEAVER_WEAVING_ENABLED = 1
};
