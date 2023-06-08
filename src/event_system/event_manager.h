#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "game_event_manager_interface.h"

enum class EventManagerInitialize {
    PROCESS_EVENTS_AT_THE_END_OF_CURRENT_FRAME,
    PROCESS_EVENTS_ON_THE_NEXT_FRAME
};

enum class EventManagerType {
    SRGB_EVENT_MANAGER_TYPE_HOTKEY,
    SRGB_EVENT_MANAGER_TYPE_PLATFORM,
    SRGB_EVENT_MANAGER_TYPE_WEAVER
};


// Events implementation ----------------------------------
struct EventHeader {
	// Contains size of the event data and type
	size_t size;
	uint32_t event_type;
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
	int GetNextEvent(uint32_t& event_type, size_t& size, void* data) {
		EventHeader const* header = reinterpret_cast<EventHeader*>(next);
		size = header->size;
		event_type = header->event_type;
        //Todo: Data is never read after assignment here.
		data = next + sizeof(EventHeader);
		next = next + sizeof(EventHeader) + size;

		// Reset the next pointer back to the start of the array
		// Maybe use modulo to reset the pointer back? Does that work?
		if (event_type == GB_EVENT_NULL) {
			ResetEventIndexPointer();
            //Todo: Return 0 if the event is invalid/does not exist at this index?
            return 0;
		}
        //Todo: Return 1 if the next even has been retrieved?
        return 1;
	}

	void ResetEventIndexPointer() {
		// Puts the next pointer back to the start of the stream buffer
		// This will be called automatically by GetNextEvent when the last event was returned
		// If the fill buffer wasn't processed by the user, this needs to be called explicitly.

		next = event_stream;
	}

	// Not entirely sure about the consts....
	const void* const GetEventStream();
};

class EventStreamWriter {
	char* const event_stream = nullptr;
	size_t used_bytes = 0;

public:

	EventStreamWriter(void* const stream) : event_stream((char*)stream) {
		// Initialize the constant stream variable so it cannot be changed in here.
	}

	void ClearStream() {
		// This function will always put a NULL_EVENT in the case no events are generated.
		// The first event that is generated, will overwrite the NULL_EVENT
		// This way we don't have to check if the first event is ok in an EventStreamReader
		// The null event will always process to the default case in a switch case (or NULL_EVENT case when a case is explicitly made for it)

		// Clearing memory is not required, we just overwrite it
		SubmitEvent(GB_EVENT_NULL, 0, 0);
		used_bytes = 0; // Set bytes to 0 after submitting so when no events are generated, only the null event is processed.
	}

	void SubmitEvent(uint32_t event_type, uint64_t size, void* data) {
		// Add event header and data to the stream
		EventHeader header{ size, event_type };
		memcpy(event_stream + used_bytes, &header, sizeof(EventHeader));
		memcpy(event_stream + used_bytes + sizeof(EventHeader), data, size);
		used_bytes += sizeof(EventHeader) + size;
	}
};

// When events are being submitted, processing should never be done
// Processing is only allowed after the EventManager has been put into a processing state
// When event should be recorded again, the EventManager should be put back in a recording/submitting state
// If we need more in the future we could use a producer/consumer strategy

class EventManager : IGameBridgeManager {
public:
    std::vector<void*> event_streams = {};

    //Todo: These methods are not implemented.
    //Returns an EventStreamReader for the given "event_manager_type".
    EventStreamReader GetEventStream(EventManagerType event_manager_type) { return {}; };

    //Returns the EventStreamWriter object for the given event_stream and event_manager_type.
    EventStreamWriter CreateEventStream(EventManagerType event_manager_type, void* event_stream) { return nullptr; };

    void PrepareForEventStreamReading() {};
    void PrepareForEventStreamWriting() {};

    GameBridgeManagerType GetEventManagerType() override {
        return this->game_bridge_manager_type;
    }
};

typedef uint32_t GameBridgeEvents;

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


// How to process events:
//EventStreamReader reader;
//void* event_data = nullptr;
//uint64_t event_type;
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
