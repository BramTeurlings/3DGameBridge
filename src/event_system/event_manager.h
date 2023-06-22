#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "game_event_manager_interface.h"

struct EventHeader {
    // Contains size of the event data and type
    size_t size;
    uint32_t event_type;
};

typedef uint32_t GameBridgeEvents;

enum class EventManagerInitialize {
    PROCESS_EVENTS_AT_THE_END_OF_CURRENT_FRAME,
    PROCESS_EVENTS_ON_THE_NEXT_FRAME
};

enum class EventManagerType {
    SRGB_EVENT_MANAGER_TYPE_HOTKEY,
    SRGB_EVENT_MANAGER_TYPE_PLATFORM,
    SRGB_EVENT_MANAGER_TYPE_WEAVER
};

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

// Reserve 0 as the NULL EVENT
constexpr size_t GB_EVENT_NULL = 0;

class EventStreamReader {
	char* const event_stream = nullptr;
	char* next = nullptr;

public:

	/*
	* Gets the next event for the user to process
	* event_type = type of the next event
	* size = size of the next event
	* data = pointer to the data of the event
	*/
	int GetNextEvent(uint32_t& event_type, size_t& size, void* data);

	void ResetEventIndexPointer();

	// Not entirely sure about the consts....
	const void* const GetEventStream();
};

class EventStreamWriter {
	char* const event_stream = nullptr;
	size_t used_bytes = 0;

public:

	EventStreamWriter(void* const stream);

	void ClearStream();

	void SubmitEvent(uint32_t event_type, uint32_t size, void* data);
};

// When events are being submitted, processing should never be done
// Processing is only allowed after the EventManager has been put into a processing state
// When event should be recorded again, the EventManager should be put back in a recording/submitting state
// If we need more in the future we could use a producer/consumer strategy

class EventManager : private IGameBridgeManager {
public:
    std::vector<void*> event_streams = {};

    // Todo: These methods are not implemented.
    // Returns an EventStreamReader for the given "event_manager_type".
    EventStreamReader* GetEventStream(EventManagerType event_manager_type);

    // Returns the EventStreamWriter object for the given event_stream and event_manager_type.
    EventStreamWriter* CreateEventStream(EventManagerType event_manager_type, void* event_stream);

    void PrepareForEventStreamReading();
    void PrepareForEventStreamWriting();

    GameBridgeManagerType GetEventManagerType() override;
};



// How to process events:
//EventStreamReader reader;
//void* event_data = nullptr;
//uint32_t event_type;
//while (reader.GetNextEvent(event_type, event_data)) {
//
//	switch (event_type) {
//	case SOME_EVENT_ENUM:
//	{
//		Do something with event_data....
//			break;
//	}
//	case NULL_EVENT: (optional..)
//	{
//		No event generated this frame...
//			break;
//	}
//	default:
//	{
//		No event generated this frame...
//			break;
//	}
//	}
//}
