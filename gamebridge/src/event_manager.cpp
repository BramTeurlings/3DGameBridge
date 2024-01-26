#include "event_manager.h"
#include <stdexcept>
#include <iostream>
#include "game_bridge_structs.h"

EventStreamReader::EventStreamReader(EventStream stream) : event_stream(stream) {
    next = event_stream.buffer.get();
}

// TODO Reserve data at the start of the buffer containing a boolean saying the stream is allowed to be read.
// TODO this way we can tell whether an EOS message has been written so it's safe to read
GB_EVENT EventStreamReader::GetNextEvent(GB_EVENT& event_type, size_t &size, void *data) {
    EventHeader const* header = reinterpret_cast<EventHeader*>(next);
    size = header->size;
    event_type = header->event_type;

    // If event type is GB_EVENT_NULL, let the next pointer point to this event
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

EventStreamWriter::EventStreamWriter(EventStream stream) : event_stream(stream) {
    // Initialize the constant stream variable so that it cannot be changed in here.
}

void EventStreamWriter::ClearStream() {
    // Clearing memory is not required, we just overwrite it
    // Submit GB_EVENT_NULL event as the first event in the stream
    used_bytes = 0;
    SubmitEvent(GB_EVENT_NULL, 0, nullptr);
    used_bytes = 0; // Set bytes to 0 again after submitting so when no events are generated, only the null event is processed.
}

// TODO reserve an extra bit of memory to fit a null event so there is always room for a null event
// TODO This part of memory can only be accessed by the event manager, which puts a null event in
bool EventStreamWriter::SubmitEvent(GB_EVENT event_type, uint32_t size, void* data) {
    // If size is larger than 0 but a nullptr was passed for data, stop execution.
    _ASSERT((size == 0 && data == nullptr) || (size > 0 && data != nullptr) && "Either size and data are 0, or size is larger than 0 and data is not nullptr.");

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
    // TODO !!! Event manager keeps a list of EventStream objects, those are different from the ones being stored in EventStreamWriters !!! This could be considered a bug !!
    // TODO should return a const pointer maybe? If this object gets edited by the manager it won't be reflected in the returned value here since it's a copy. Could be a design decision for easier multi threading perhaps
    return event_stream;
}

size_t EventStreamWriter::GetUsedBytes()
{
    return used_bytes;
}

// EventManager
std::shared_ptr<EventStreamReader> EventManager::GetEventStreamReader(GameBridgeEventManagerType event_manager_type) {
    // TODO Use indirection array to get stream readers and writers?
    try {
        EventStream const& stream = event_streams.at(event_manager_type);
        std::shared_ptr<EventStreamReader> reader = std::make_shared<EventStreamReader>(EventStreamReader(stream));
        stream_readers.push_back(reader);
        return reader;
    }
    catch (std::out_of_range&){
        std::cout << "Couldn't find existing stream" << "\n";
        return std::shared_ptr<EventStreamReader>{};
    }
    catch (std::exception& error) {
        std::cout << "Exception occurred while getting even stream: " << error.what() << "\n";
        return std::shared_ptr<EventStreamReader>{};
    }
}

// TODO maybe use error codes?
// TODO Only one event stream can exists per manager, the existing stream will be returned or nullptr?
std::shared_ptr<EventStreamWriter> EventManager::CreateEventStream(GameBridgeEventManagerType event_manager_type, uint32_t max_event_count, size_t extra_event_data_size)
{
    size_t message_size = sizeof(EventHeader) + extra_event_data_size;
    size_t buffer_size = message_size * (max_event_count + 1); // +1 to event count for end of buffer message

    std::shared_ptr<char[]> ptr(new char[buffer_size]);
    EventStream stream{ buffer_size, buffer_size - sizeof(EventHeader),static_cast<uint32_t>(event_manager_type), ptr};
    event_streams.insert({ event_manager_type, stream });

    std::shared_ptr<EventStreamWriter> writer = std::make_shared<EventStreamWriter>(EventStreamWriter(stream));
    stream_writers.push_back(writer);
    // Initialize the stream with a NULL_EVENT
    writer->ClearStream();

    return writer;
}

void EventManager::PrepareForEventStreamProcessing()
{
    // Add NULL_EVENT at the end of every buffer
    for(const auto& writer : stream_writers)
    {
        auto stream = writer->GetEventStream();
        size_t used_bytes = writer->GetUsedBytes();

        // Cannot fit NULL_EVENT at the end of the buffer, if this executes there is a bug somewhere.
        size_t remaining_memory = stream.buffer_size - writer->GetUsedBytes();
        _ASSERT(remaining_memory >= sizeof(EventHeader) && "There should always be more remaining memory than the size of an event header at this point");

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

void EventManager::PrepareForEventStreamSubmission()
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

/** TODO implement this later, remove the event stream from the event manager.
 * An END OF STREAM message should be sent and all reader should the be discarded. The shared pointer of the stream will be discarded then too.
*/
// void EndEventStream(EventStreamWriter writer);

GameBridgeManagerType EventManager::GetEventManagerType() {
    return GameBridgeManagerType::GB_MANAGER_EVENTS;
}
