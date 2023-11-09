#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <memory>
#include "game_bridge_manager_interface.h"
#include "game_bridge_structs.h"

#define DEFAULT_MESSAGE_SIZE 0
#define DEFAULT_MESSAGE_COUNT 300

struct EventHeader {
    // Contains size of the event data and type
    const size_t size;
    const uint32_t event_type;
};

struct EventStream {
    //Todo: Removed const here due to construction problems with the event_streams object. Check if this is ok.
	size_t stream_size;
    //Todo: Removed const here due to construction problems with the event_streams object. Check if this is ok.
    uint32_t max_event_count;
    //Todo: Removed const here due to construction problems with the event_streams object. Check if this is ok.
    std::shared_ptr<char[]> stream;
	uint32_t stream_id; // Used for event type for now, later used as actual id
};
// Reserve 0 as the NULL EVENT
constexpr size_t GB_EVENT_NULL = 0;

class GAME_BRIDGE_API EventStreamReader {
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
	// TODO Also not sure we still want this
	// const void* const GetEventStream();
};

class GAME_BRIDGE_API EventStreamWriter {
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

class GAME_BRIDGE_API EventManager : private IGameBridgeManager {
private:
    std::unordered_map<EventManagerType, EventStream, ClassHash> event_streams = {};

public:
	// Probably not necessary anymore
    // std::vector<EventStreamReader> stream_readers = {};
    // std::vector<EventStreamWriter> stream_writers = {};
	// Use indirection array to get stream readers and writers

	//TODO need some way to tell when the frame begins and ends to every stream reader and writer
	//TODO They can check themselves if the stream they read/write still exists. return a "NULL" message when the stream has ended
    // Returns an EventStreamReader for the given "event_manager_type".
	bool EventManager::GetEventStream(EventManagerType event_manager_type, EventStreamReader& stream_reader);

    // Returns the EventStreamWriter object for the given event_stream and event_manager_type.
	// TODO message size calculation is wrong. Is message size the extra data after the header? And header is always included
    EventStreamWriter CreateEventStream(EventManagerType event_manager_type, size_t message_size = DEFAULT_MESSAGE_SIZE, uint32_t max_message_count = DEFAULT_MESSAGE_COUNT);

    void PrepareForEventStreamReading();
    void PrepareForEventStreamWriting();

	// TODO implement this later, remove the event stream from the event manager.
	// An END OF STREAM message should be sent and all reader should theb be discarded. The shared pointer of the stream will be discarded then too.
	//void EndEventStream(EventStreamWriter writer);

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
