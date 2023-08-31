#include "event_manager.h"
#include "game_bridge_structs.h"

// How to process events:
void ProcessEvents() {
        EventStreamReader reader;
        void* event_data = nullptr;
        uint32_t event_type = 1;
        size_t data_size = 0;
        while (reader.GetNextEvent(event_type, data_size, event_data)) {

            switch (event_type) {
                // Todo: Is this right?
                case (uint32_t)HotKeyEvent::GB_EVENT_HOTKEY_TURN_OFF_LENS:
                {
                    // Do something with event_data....
                    break;
                }
                case GB_EVENT_NULL: //(optional..)
                    {
                        // No event generated this frame...
                        break;
                    }
                default:
                {
                    // No event generated this frame...
                    break;
                }
            }
        }
};

EventStreamReader::EventStreamReader(EventStream stream) : event_stream((stream)) {

}

// EventStreamReader
int EventStreamReader::GetNextEvent(uint32_t &event_type, size_t &size, void *data) {
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

void EventStreamReader::ResetEventIndexPointer() {
    // Puts the next pointer back to the start of the stream buffer
    // This will be called automatically by GetNextEvent when the last event was returned
    // If the fill buffer wasn't processed by the user, this needs to be called explicitly.

    next = event_stream;
}

const void *const EventStreamReader::GetEventStream() {
    return event_stream;
}
// End EventStreamReader

// EventStreamWriter
EventStreamWriter::EventStreamWriter(EventStream stream) : event_stream((stream)) {
    // Initialize the constant stream variable so that it cannot be changed in here.
}

void EventStreamWriter::ClearStream() {
    // This function will always put a NULL_EVENT in the case no events are generated.
    // The first event that is generated, will overwrite the NULL_EVENT
    // This way we don't have to check if the first event is ok in an EventStreamReader
    // The null event will always process to the default case in a switch case (or NULL_EVENT case when a case is explicitly made for it)

    // Clearing memory is not required, we just overwrite it
    SubmitEvent(GB_EVENT_NULL, 0, 0);
    used_bytes = 0; // Set bytes to 0 after submitting so when no events are generated, only the null event is processed.
}

void EventStreamWriter::SubmitEvent(uint32_t event_type, uint32_t size, void *data) {
    // Add event header and data to the stream
    EventHeader header{ size, event_type };
    memcpy(event_stream + used_bytes, &header, sizeof(EventHeader));
    memcpy(event_stream + used_bytes + sizeof(EventHeader), data, size);
    used_bytes += sizeof(EventHeader) + size;
}
// End EventStreamReader

// EventManager
EventStreamReader *EventManager::GetEventStream(EventManagerType event_manager_type) { 
    size_t size = event_streams.size();
    uint32_t stream_idx = 0;
    bool found = false;
    for (int i = 0; i < size; i++) {
        if (event_streams[i].stream_id == (uint32_t)event_manager_type) {
            stream_idx = i;
            found = true;
        }
    }

    if (found) {
        EventStreamReader reader (event_streams[stream_idx]);
        stream_readers.push_back(reader);
        return 
    }

    return {}; 
}

EventStreamWriter *EventManager::CreateEventStream(EventManagerType event_manager_type, size_t message_size, uint32_t max_message_count) {
    EventStream stream{ message_size, max_message_count, new char[message_size * max_message_count], (uint32_t)event_manager_type };
    event_streams.push_back(stream);

    stream_writers.push_back(EventStreamWriter(stream));
    return nullptr;
}

void EventManager::PrepareForEventStreamReading() {}

void EventManager::PrepareForEventStreamWriting() {}

GameBridgeManagerType EventManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_EVENTS;
}
