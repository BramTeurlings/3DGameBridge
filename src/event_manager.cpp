#include "event_manager.h"
#include "game_bridge_structs.h"
#include <stdexcept>
#include <iostream>

EventStreamReader::EventStreamReader(): event_stream({})
{
}

EventStreamReader::EventStreamReader(EventStream stream) : event_stream((stream)) {
    next = event_stream.buffer.get();
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

    next = event_stream.buffer.get();
}

//const void *const EventStreamReader::GetEventStream() {
//    return event_stream;
//}
// End EventStreamReader

EventStreamWriter::EventStreamWriter()
{
}

// EventStreamWriter
EventStreamWriter::EventStreamWriter(EventStream stream) : event_stream(stream) {
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
    current_event_count = 0;
}

// TODO reserve an extra bit of memory to fit a null event so there is always room for a null event
// TODO This part of memory can only be accessed by the event manager, which puts a null event in
bool EventStreamWriter::SubmitEvent(GB_EVENT event_type, uint32_t size, void* data) {
    size_t remaining_memory = event_stream.buffer_size - used_bytes;
    if (remaining_memory < sizeof(EventHeader) + size)
    {
        return false;
    }

    // Add event header and data to the stream
    EventHeader header{ size, event_type };
    memcpy(event_stream.buffer.get() + used_bytes, &header, sizeof(EventHeader));
    memcpy(event_stream.buffer.get() + used_bytes + sizeof(EventHeader), data, size);
    used_bytes += sizeof(EventHeader) + size;
    current_event_count++;

    return true;
}

EventStream EventStreamWriter::GetEventStream()
{
    return event_stream;
}

size_t EventStreamWriter::GetUsedBytes()
{
    return used_bytes;
}

// End EventStreamReader

EventManager::EventManager()
{
}

// EventManager
bool EventManager::GetEventStreamReader(EventManagerType event_manager_type, EventStreamReader& stream_reader) {
    try {
        EventStream const& stream = event_streams.at(event_manager_type);
        stream_reader = EventStreamReader (stream);
        return true;
    }
    catch (std::runtime_error& error){
        std::cout << "Couldn't find existing stream" << "\n";
        return false;
    }
}

EventStreamWriter EventManager::CreateEventStream(EventManagerType event_manager_type, uint32_t max_event_count, size_t extra_event_data_size)
{
    size_t message_size = sizeof(EventHeader) + extra_event_data_size;
    size_t buffer_size = message_size * max_event_count;

    std::shared_ptr<char[]> ptr(new char[buffer_size]);
    EventStream stream{ buffer_size, max_event_count,static_cast<uint32_t>(event_manager_type), ptr };
    event_streams.insert({ event_manager_type, stream });

    return EventStreamWriter(stream);
}
void EventManager::PrepareForEventStreamReading() {}

void EventManager::PrepareForEventStreamWriting() {}

GameBridgeManagerType EventManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_EVENTS;
}
