#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "game_bridge_manager_interface.h"
#include "game_bridge_structs.h"

#define DEFAULT_MESSAGE_SIZE 4
#define DEFAULT_MESSAGE_COUNT 300

struct EventHeader {
    // Contains size of the event data and type
    const size_t size;
    const uint32_t event_type;
};

struct EventStream {
	const size_t stream_size;
	const uint32_t max_event_count;
	char* const stream;
	uint32_t stream_id; // Used for event type for now, later used as actual id
};
// Reserve 0 as the NULL EVENT
constexpr size_t GB_EVENT_NULL = 0;

class EventStreamReader {
	const EventStream event_stream;
	char* next = nullptr;

public:
	EventStreamReader(EventStream stream);

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
	const EventStream event_stream;
	size_t used_bytes = 0;

public:
	EventStreamWriter(EventStream stream);

	void ClearStream();

	void SubmitEvent(uint32_t event_type, uint32_t size, void* data);
};

// When events are being submitted, processing should never be done
// Processing is only allowed after the EventManager has been put into a processing state
// When event should be recorded again, the EventManager should be put back in a recording/submitting state
// If we need more in the future we could use a producer/consumer strategy

class EventManager : private IGameBridgeManager {
private:
    std::vector<EventStream> event_streams = {};

public:
    std::vector<EventStreamReader> stream_readers = {};
    std::vector<EventStreamWriter> stream_writers = {};
	// Use indirection array to get stream readers and writers

	//TODO need some way to tell when the frame begins and ends to every stream reader and writer
	//TODO They can check themselves if the stream they read/write still exists. return a "NULL" message when the stream has ended
    // Returns an EventStreamReader for the given "event_manager_type".
    EventStreamReader* GetEventStream(EventManagerType event_manager_type);

    // Returns the EventStreamWriter object for the given event_stream and event_manager_type.
    EventStreamWriter* CreateEventStream(EventManagerType event_manager_type, size_t message_size = DEFAULT_MESSAGE_SIZE, uint32_t max_message_count = DEFAULT_MESSAGE_COUNT);

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
