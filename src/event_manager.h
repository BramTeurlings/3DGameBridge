#pragma once
#include <cstdint>
#include <memory>

// Reserve 0 as the NULL EVENT
#define GB_EVENT_NULL 0

typedef uint32_t GameBridgeEvents;

// 0 Should be reserved for null event
enum HotKeyEvent {
	GB_EVENT_HOTKEY_TURN_ON_LENS = 1,
	GB_EVENT_HOTKEY_TURN_OFF_LENS
};

// Events implementation ----------------------------------
struct EventHeader {
	// Contains size of the event data and type
	size_t size;
	uint32_t event_type;
};

class EventStreamReader {
	char* const event_stream = nullptr;
	char* next = nullptr;

	/*
	* Gets the next event for the user to process
	* event_type = type of the next event
	* size = size of the next event
	* data = pointer to the data of the event
	*/
	void GetNextEvent(uint32_t& event_type, size_t& size, void* data) {
		EventHeader* header = reinterpret_cast<EventHeader*>(next);
		size = header->size;
		event_type = header->event_type;
		data = next + sizeof(EventHeader);
		next = next + sizeof(EventHeader) + size;

		// Reset the next pointer back to the start of the array
		// Maybe use modulo to reset the pointer back? Does that work?
		if (event_type == GB_EVENT_NULL) {
			ResetEventIndexPointer();
		}
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

	void SubmitEvent(uint64_t event_type, uint64_t size, void* data) {
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
