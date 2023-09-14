#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <memory>
#include "game_bridge_manager_interface.h"
#include "game_bridge_structs.h"
#include <variant>

#define DEFAULT_MESSAGE_SIZE 0
#define DEFAULT_MESSAGE_COUNT 300

struct GAME_BRIDGE_API EventHeader {
    // Contains size of the event data and type
	// TODO lower amount of bytes stored
    size_t size;
    uint32_t event_type;
};

// const data members makes the struct only copy constructable, not copy assignable
struct GAME_BRIDGE_API EventStream {
	size_t buffer_size;
	size_t max_event_count;
	size_t stream_id; // Used for event type for now, later used as actual id
	std::shared_ptr<char[]> buffer;
};
// Reserve 0 as the NULL EVENT
constexpr size_t GB_EVENT_NULL = 0;

class GAME_BRIDGE_API EventStreamReader {
	EventStream event_stream;
	char* next = nullptr;

public:
	EventStreamReader();
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
	EventStream event_stream {};
	size_t used_bytes = 0;
	size_t current_event_count = 0;

public:
	EventStreamWriter();
	EventStreamWriter(EventStream stream);

	void ClearStream();

	bool SubmitEvent(GB_EVENT event_type, uint32_t size, void* data);
	EventStream GetEventStream();
	size_t GetUsedBytes();
};

// When events are being submitted, processing should never be done
// Processing is only allowed after the EventManager has been put into a processing state
// When event should be recorded again, the EventManager should be put back in a recording/submitting state
// If we need more in the future we could use a producer/consumer strategy

class GAME_BRIDGE_API EventManager : private IGameBridgeManager {
private:
    std::unordered_map<uint32_t, EventStream, ClassHash> event_streams = {};

public:

	EventManager();

	// Probably not necessary anymore
    // std::vector<EventStreamReader> stream_readers = {};
    // std::vector<EventStreamWriter> stream_writers = {};
	// Use indirection array to get stream readers and writers

	//TODO need some way to tell when the frame begins and ends to every stream reader and writer
	//TODO They can check themselves if the stream they read/write still exists. return a "NULL" message when the stream has ended
    // Returns an EventStreamReader for the given "event_manager_type".
	bool GetEventStreamReader(EventManagerType event_manager_type, EventStreamReader& stream_reader);

    // Creates an the EventStreamWriter object with an underlying EvenStream for the given event_manager_type.
	//
	
    EventStreamWriter CreateEventStream(EventManagerType event_manager_type, uint32_t max_event_count = DEFAULT_MESSAGE_COUNT, size_t extra_event_data_size = DEFAULT_MESSAGE_SIZE);

    void PrepareForEventStreamReading();
    void PrepareForEventStreamWriting();

	// TODO implement this later, remove the event stream from the event manager.
	// An END OF STREAM message should be sent and all reader should theb be discarded. The shared pointer of the stream will be discarded then too.
	//void EndEventStream(EventStreamWriter writer);

    GameBridgeManagerType GetEventManagerType() override;
};
