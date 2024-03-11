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
/*
 * Struct holding the underlying event stream
 */
struct GAME_BRIDGE_API EventStream {
    // Size of the entire buffer.
    size_t buffer_size = 0;

    // Size of the buffer used for user events.
    // This size will always be smaller than buffer_size so system messages can always be added.
    size_t user_buffer_size = 0;

    // Used for event type for now, later used as actual id
    uint32_t stream_id = 0;

    // Underlying event buffer
    std::shared_ptr<char[]> buffer;
};

class GAME_BRIDGE_API EventStreamReader {
    EventStream event_stream;
    char* next = nullptr;

public:
    /**
     * \brief Create Event Stream Reader
     * \param stream EventStream
     */
    explicit EventStreamReader(EventStream stream);

    /**
    * \brief Gets the next event for the user to process
    * @param event_type Type of the next event
    * \return pointer to the event, nullptr if event_type is GB_NULL_EVENT
    */
    void* GetNextEvent(uint32_t& event_type);

    /**
     * \brief Sets the read index back to the beginning of the buffer.
     */
    void ResetEventIndexPointer();
};

class GAME_BRIDGE_API EventStreamWriter {
    EventStream event_stream {};
    size_t used_bytes = 0;

public:
    /**
     * \brief Create EventStreamWriter
     * \param stream EventStream
     */
    explicit EventStreamWriter(EventStream stream);

    /**
     * \brief Clears the event stream buffer.
     * \details After clearing, the first event will be a NULL_EVENT. Any event submitted after clearing will overwrite the NULL_EVENT.
     * ClearStream is used when PrepareForEventStreamSubmission is called on the EventManager
     * The null event will always process to the default case in a switch case (or NULL_EVENT case when a case is explicitly made for it)
    */
    void ClearStream();

    /**
     * \brief Submit event to the event stream
     * \param event_type    Event type enum
     * \param size          Size of the extra data of the message. This can be any size regardless of the extra message size the buffer was create with
     * \param data          Extra data pointer
     * \return True when submission succeeded
     * \return False when the event doesn't fit the buffer
     */
    bool SubmitEvent(GB_EVENT event_type, uint32_t size = 0, void* data = nullptr);

    /**
     * \brief Get the underlying EventStream
     * \return Underlying EventStream
     */
    EventStream GetEventStream();

    /**
     * \brief Gets the amount of bytes in use
     * \details This will be reset to 0 when calling PrepareForEventStreamSubmission os called in the EventManager or when calling ClearStream() 
     * \return Number of bytes in use
     */
    size_t GetUsedBytes();
};

class GAME_BRIDGE_API EventManager : private IGameBridgeManager {
    // If we need more in the future we could use a producer/consumer strategy

    std::unordered_map<uint32_t, EventStream> event_streams = {};
    std::vector<std::shared_ptr<EventStreamReader>> stream_readers = {};
    std::vector<std::shared_ptr<EventStreamWriter>> stream_writers = {};

public:
    /**
     * \brief Returns an event stream reader for the given event manager type.
     * @param event_stream_type    type of the event manager to get a processing stream of.
     * \return Shared pointer to an EventStreamReader
     */
    std::shared_ptr<EventStreamReader> GetEventStreamReader(EventStreamType event_stream_type);

    /**
     * \brief   Creates an EventStreamWriter object with an underlying EvenStream for the given GameBridgeEventManagerType
     * \details The underlying buffer will be made larger than the given buffer size to fit extra system messages.
     * Buffer layout will look like: <[Header][Extra Message Data]> ..... <[NULL_EVENT]>. Buffer will always end with a NULL_EVENT.
     * @param event_stream_type        Type of the event manager to create an event stream for
     * @param max_event_count           Number of events that have to fit in the buffer
     * @param extra_event_data_size     Size of extra data that will be added per event
     * \return Shared pointer to an EventStreamReader
     * \return Returns nullptr when no even stream exist for the event_manager_type
    */
    std::shared_ptr<EventStreamWriter> CreateEventStream(EventStreamType event_stream_type, uint32_t max_event_count = DEFAULT_MESSAGE_COUNT, size_t extra_event_data_size = DEFAULT_MESSAGE_SIZE);

    /**
     * \brief   Prepare event streams for processing of the events.
     * \details Should always be called before any events are being processed by the application
     */
    void PrepareForEventStreamProcessing();

    /**
     * \brief Prepare event streams for submission of event streams
     */
    void PrepareForEventStreamSubmission();

    /**
     * \brief Gets GameBridgeManagerType
     * \return GameBridgeManagerType
     */
    GameBridgeManagerType GetEventManagerType() override;
};
