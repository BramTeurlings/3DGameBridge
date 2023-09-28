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
GB_EVENT EventStreamReader::GetNextEvent(GB_EVENT& event_type, size_t &size, void *data) {
    EventHeader const* header = reinterpret_cast<EventHeader*>(next);
    size = header->size;
    event_type = header->event_type;

    // If event type is GB_EVENT_NULL, let the next pointer  point to this event
    if (event_type == GB_EVENT_NULL) {
        return event_type;
    }

    // TODO Data is never read after assignment here.
    data = next + sizeof(EventHeader);
    next = next + sizeof(EventHeader) + size;

    return event_type;
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
    // This function will always write a NULL_EVENT in the case no events are generated.
    // The first event that is generated, will overwrite the NULL_EVENT
    // This way we don't have to check if the first event is ok in an EventStreamReader
    // The null event will always process to the default case in a switch case (or NULL_EVENT case when a case is explicitly made for it)

    // Clearing memory is not required, we just overwrite it
    // Submit GB_EVENT_NULL event as the first event in the stream
    used_bytes = 0;
    SubmitEvent(GB_EVENT_NULL, 0, nullptr);
    used_bytes = 0; // Set bytes to 0 again after submitting so when no events are generated, only the null event is processed.
}

// TODO reserve an extra bit of memory to fit a null event so there is always room for a null event
// TODO This part of memory can only be accessed by the event manager, which puts a null event in
bool EventStreamWriter::SubmitEvent(GB_EVENT event_type, uint32_t size, void* data) {
    size_t remaining_memory = event_stream.user_buffer_size - used_bytes;
    if (remaining_memory < sizeof(EventHeader) + size)
    {
        return false;
    }

    // Add event header and data to the stream
    EventHeader header{ size, event_type };
    memcpy(event_stream.buffer.get() + used_bytes, &header, sizeof(EventHeader));
    memcpy(event_stream.buffer.get() + used_bytes + sizeof(EventHeader), data, size);
    used_bytes += sizeof(EventHeader) + size;
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
std::shared_ptr<EventStreamReader> EventManager::GetEventStreamReader(EventManagerType event_manager_type) {
    try {
        EventStream const& stream = event_streams.at(event_manager_type);
        std::shared_ptr<EventStreamReader> reader = std::make_shared<EventStreamReader>(EventStreamReader(stream));
        stream_readers.push_back(reader);
        return reader;
    }
    catch (std::runtime_error& error){
        std::cout << "Couldn't find existing stream" << "\n";
        return std::shared_ptr<EventStreamReader>{};
    }
}

//TODO maybe use error codes?
std::shared_ptr<EventStreamWriter> EventManager::
CreateEventStream(EventManagerType event_manager_type, uint32_t max_event_count, size_t extra_event_data_size)
{
    size_t message_size = sizeof(EventHeader) + extra_event_data_size;
    size_t buffer_size = message_size * (max_event_count + 1); // +1 to event count for end of buffer message

    std::shared_ptr<char[]> ptr(new char[buffer_size]);
    EventStream stream{ buffer_size, buffer_size - sizeof(EventHeader),static_cast<uint32_t>(event_manager_type), ptr};
    event_streams.insert({ event_manager_type, stream });

    std::shared_ptr<EventStreamWriter> writer = std::make_shared<EventStreamWriter>(EventStreamWriter(stream));
    stream_writers.push_back(writer);

    return writer;
}

void EventManager::PrepareForEventStreamReading()
{
    // Add final event message
    for(const auto& writer : stream_writers)
    {
        auto stream = writer->GetEventStream();
        size_t used_bytes = writer->GetUsedBytes();

        size_t remaining_memory = stream.buffer_size - writer->GetUsedBytes();
        if (remaining_memory < sizeof(EventHeader))
        {
            throw std::exception("No memory left in the buffer for the event manager");
        }

        // Add event header and data to the stream
        EventHeader header{ 0, GB_EVENT_NULL };
        memcpy(stream.buffer.get() + used_bytes, &header, sizeof(EventHeader));
    }

    // Reset read pointer
    for (const auto& reader : stream_readers)
    {
        reader->ResetEventIndexPointer();
    }
}

void EventManager::PrepareForEventStreamWriting()
{
    // Makes sure the first event in the stream is always a GB_EVENT_NULL
    for (const auto& writer : stream_writers)
    {
        writer->ClearStream();
    }

    // Reset read pointer
    for (const auto& reader : stream_readers)
    {
        reader->ResetEventIndexPointer();
    }
}

GameBridgeManagerType EventManager::GetEventManagerType() {
    return GameBridgeManagerType::SRGB_MANAGER_EVENTS;
}
