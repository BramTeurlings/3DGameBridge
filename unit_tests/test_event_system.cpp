//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"
#include "event_manager.h"

#define TEST_EVENT_COUNT 500

class EventSystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        platform_event_writer = event_manager.CreateEventStream(SRGB_EVENT_MANAGER_TYPE_PLATFORM, TEST_EVENT_COUNT, 0);
        hotkey_event_writer = event_manager.CreateEventStream(SRGB_EVENT_MANAGER_TYPE_HOTKEY, TEST_EVENT_COUNT, 0);
        weaver_event_writer = event_manager.CreateEventStream(SRGB_EVENT_MANAGER_TYPE_WEAVER, TEST_EVENT_COUNT, 0);
    }

    void FillEventStreams()
    {
        // Fill event streams
        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr);
        }

        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            hotkey_event_writer->SubmitEvent(GB_EVENT_HOTKEY_TOGGLE_LENS, 0, nullptr);
        }

        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            weaver_event_writer->SubmitEvent(SRGB_EVENT_WEAVER_WEAVING_ENABLED, 0, nullptr);
        }
    }

    void SetUpEvenStreamReaders()
    {
        ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_HOTKEY));
        ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_WEAVER));
        ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM));
    }

    EventManager event_manager;

    std::shared_ptr<EventStreamWriter> platform_event_writer;
    std::shared_ptr<EventStreamWriter> hotkey_event_writer;
    std::shared_ptr<EventStreamWriter> weaver_event_writer;

    std::shared_ptr <EventStreamReader> platform_event_reader;
    std::shared_ptr <EventStreamReader> hotkey_event_reader;
    std::shared_ptr <EventStreamReader> weaver_event_reader;

    // Create mock event types
    enum TestEnums
    {
        TEST_NULL,
        TEST_1,
        TEST_2,
        TEST_3,
        TEST_4,
        TEST_5,
        TEST_6,
        TEST_7,
        TEST_8,
        TEST_9
    };
};

TEST_F(EventSystemTests, GetWriterEventStream) {
    EventStream stream = platform_event_writer->GetEventStream();
    ASSERT_NE(stream.buffer, nullptr);
}

TEST_F(EventSystemTests, EventSystem) {

    // Check platform writer
    EventStream stream = platform_event_writer->GetEventStream();
    ASSERT_GE(stream.buffer_size, 1);

    // Check hotkey writer
    stream = hotkey_event_writer->GetEventStream();
    ASSERT_GE(stream.buffer_size, 1);

    // Check weaver writer
    stream = weaver_event_writer->GetEventStream();
    ASSERT_GE(stream.buffer_size, 1);
}

TEST_F(EventSystemTests, GetUsedBytes)
{
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), 0);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_TRUE(platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 5);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_TRUE(platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 10);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_TRUE(platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 15);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_TRUE(platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 20);
}

TEST_F(EventSystemTests, SubmitEvent) {
    // Fill event stream
    for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
        ASSERT_TRUE(platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    // Check if 500 enums can be stored
    EventStream stream = platform_event_writer->GetEventStream();
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * TEST_EVENT_COUNT);

    ASSERT_FALSE(platform_event_writer->SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr))
    << "Check if not more than 500 enums can be stored, SubmitEvent should return false";

    // TODO test with extra data
}

TEST_F(EventSystemTests, GetEventStreamReader) {
    platform_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_HOTKEY);
    hotkey_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_WEAVER);
    weaver_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM);

    ASSERT_TRUE(platform_event_reader);
    ASSERT_TRUE(hotkey_event_reader);
    ASSERT_TRUE(weaver_event_reader);
}

TEST_F(EventSystemTests, GetNextEvent) {
    // Get event stream reader
    platform_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM);

    // Write 499 mock events to stream
    for (uint32_t i = 0; i < TEST_EVENT_COUNT-1; i++) {
        int used_event = i % 3;

        switch (used_event)
        {
        case 0:
            ASSERT_TRUE(platform_event_writer->SubmitEvent(TEST_1, 0, nullptr)) << "Not enough memory to write to event buffer, index: " << i;
            break;
        case 1:
            ASSERT_TRUE(platform_event_writer->SubmitEvent(TEST_2, 0, nullptr)) << "Not enough memory to write to event buffer, index: " << i;
            break;
        case 2:
            ASSERT_TRUE(platform_event_writer->SubmitEvent(TEST_3, 0, nullptr)) << "Not enough memory to write to event buffer, index: " << i;
            break;
        }
    }

    // Write 500th event
    ASSERT_TRUE(platform_event_writer->SubmitEvent(TEST_NULL, 0, nullptr));

    // Read events from stream
    uint32_t eventidx = 0;

    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;
    while (platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data)) {
        switch (event_type) {
            case TEST_1:
            {
                ASSERT_EQ(event_type, TEST_1) << "Event type not TEST_1, index: " << eventidx;
                break;
            }
            case TEST_2:
            {
                ASSERT_EQ(event_type, TEST_2) << "Event type not TEST_2, index: " << eventidx;
                break;
            }
            case TEST_3:
            {
                ASSERT_EQ(event_type, TEST_3) << "Event type not TEST_3, index: " << eventidx;
                break;
            }
            default:
            {
                // 0 was already returned by GetNextEvent so the while loop quits before coming here
                ASSERT_EQ(event_type, TEST_NULL) << "Event type not TEST_NULL, SHOULD NOT COME HERE, index: " << eventidx;
                break;
            }
        }

        eventidx++;
    }

    // Test last event
    ASSERT_EQ(event_type, TEST_NULL) << "Event type not TEST_NULL";
    eventidx++; // Increment the last index

    ASSERT_EQ(eventidx, TEST_EVENT_COUNT);
}

// TODO test after EventStreamWriter tests and use a writer inside this test if that makes sense
TEST_F(EventSystemTests, ClearStream) {
    platform_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM);
    auto buffer = platform_event_writer->GetEventStream().buffer;

    {
        // Submit 5 times TEST_5
        for (int i = 0; i < 5; i++) {
            platform_event_writer->SubmitEvent(TEST_5);
        }

        // Bytes used should be 0
        platform_event_writer->ClearStream();
        ASSERT_EQ(platform_event_writer->GetUsedBytes(), 0)
            << "Bytes used should be 0";
    }

    {
        // Submit TEST_1 and get the event header
        platform_event_writer->SubmitEvent(TEST_1);
        EventHeader* header = reinterpret_cast<EventHeader*>(buffer.get());

        // Clear then submit TEST_2
        platform_event_writer->ClearStream();
        platform_event_writer->SubmitEvent(TEST_2);

        // TEST_2 should be the first event in the stream
        ASSERT_EQ(header->event_type, TEST_2)
            << "TEST_2 should be the first event in the stream";

        // Clear then submit TEST_3
        platform_event_writer->ClearStream();
        platform_event_writer->SubmitEvent(TEST_3);

        // TEST_3 should be the first event in the stream
        ASSERT_EQ(header->event_type, TEST_3)
            << "TEST_3 should be the first event in the stream";

        // Clear then submit TEST_4
        platform_event_writer->ClearStream();
        platform_event_writer->SubmitEvent(TEST_4);

        // TEST_4 should be the first event in the stream
        ASSERT_EQ(header->event_type, TEST_4)
            << "TEST_4 should be the first event in the stream";
    }

    platform_event_writer->ClearStream();
    platform_event_reader->ResetEventIndexPointer();

    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;
    {
        for (int i = 0; i < 5; i++) {
            platform_event_writer->SubmitEvent(TEST_1);
        }

        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_1) << "Event not equal to TEST_1";

        // Make sure TEST_NULL is the only event returned after clearing
        platform_event_writer->ClearStream();
        platform_event_reader->ResetEventIndexPointer();
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
    }

    {
        platform_event_writer->SubmitEvent(TEST_2);
        platform_event_writer->ClearStream();
        platform_event_reader->ResetEventIndexPointer();
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";

        platform_event_writer->ClearStream();
        platform_event_reader->ResetEventIndexPointer();
        platform_event_writer->SubmitEvent(TEST_3);
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_3) << "Event not equal to TEST_3";
    }
}

TEST_F(EventSystemTests, ResetEventIndexPointer) {
    platform_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM);

    platform_event_writer->SubmitEvent(TEST_1);
    platform_event_writer->SubmitEvent(TEST_2);
    platform_event_writer->SubmitEvent(TEST_3);
    platform_event_writer->SubmitEvent(TEST_4);
    platform_event_writer->SubmitEvent(TEST_5);
    platform_event_writer->SubmitEvent(TEST_6);
    platform_event_writer->SubmitEvent(TEST_7);
    platform_event_writer->SubmitEvent(TEST_8);
    platform_event_writer->SubmitEvent(TEST_9);
    platform_event_writer->SubmitEvent(TEST_NULL);

    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;

    // Test reset function from multiple points in the buffer
    for (int i = 1; i <= 5; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
        << "ResetEventIndexPointer: Event not equal to " << i;
    }

    platform_event_reader->ResetEventIndexPointer();
    for (int i = 1; i <= 6; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "ResetEventIndexPointer: Event not equal to " << i;
    }

    platform_event_reader->ResetEventIndexPointer();
    for (int i = 1; i <= 7; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "ResetEventIndexPointer: Event not equal to " << i;
    }

    platform_event_reader->ResetEventIndexPointer();
    for (int i = 1; i <= 8; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "ResetEventIndexPointer: Event not equal to " << i;
    }

    platform_event_reader->ResetEventIndexPointer();
    for (int i = 1; i <= 9; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "ResetEventIndexPointer: Event not equal to " << i;
    }

    // Final event should be TEST_NULL
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL)
    << "Final event should be TEST_NULL";

    // Make sure the event index pointer is reset to the start of the buffer
    platform_event_reader->ResetEventIndexPointer();
    auto buffer = platform_event_writer->GetEventStream().buffer;
    EventHeader* header = reinterpret_cast<EventHeader*>(buffer.get());
    GB_EVENT next_event = platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data);
    ASSERT_EQ(next_event, header->event_type)
    << "Event index should be the begin of the event buffer";
}

TEST_F(EventSystemTests, FullTest)
{
    // Get event stream reader
    platform_event_reader = event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM);

    // Prepare for writing events
    event_manager.PrepareForEventStreamSubmission();

    // Write 499 mock events to stream
    // Try 50 more the make sure only 500 are written, and the other 50 did not write to the stream.

    for (uint32_t i = 0; i < TEST_EVENT_COUNT + 50; i++) {
        int used_event = i % 8;

        switch (used_event)
        {
        case 0:
            platform_event_writer->SubmitEvent(TEST_1, 0, nullptr);
            break;
        case 1:
            platform_event_writer->SubmitEvent(TEST_2, 0, nullptr);
            break;
        case 2:
            platform_event_writer->SubmitEvent(TEST_3, 0, nullptr);
            break;
        case 3:
            platform_event_writer->SubmitEvent(TEST_4, 0, nullptr);
            break;
        case 4:
            platform_event_writer->SubmitEvent(TEST_5, 0, nullptr);
            break;
        case 5:
            platform_event_writer->SubmitEvent(TEST_6, 0, nullptr);
            break;
        case 6:
            platform_event_writer->SubmitEvent(TEST_7, 0, nullptr);
            break;
        case 7:
            platform_event_writer->SubmitEvent(TEST_8, 0, nullptr);
            break;
        case 8:
            platform_event_writer->SubmitEvent(TEST_9, 0, nullptr);
            break;
        }
    }

    // Prepare for reading events
    event_manager.PrepareForEventStreamProcessing();

    uint32_t eventidx = 0;

    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;
    while (platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data)) {
        switch (event_type) {
        case TEST_1:
        {
            ASSERT_EQ(event_type, TEST_1);
            break;
        }
        case TEST_2:
        {
            ASSERT_EQ(event_type, TEST_2);
            break;
        }
        case TEST_3:
        {
            ASSERT_EQ(event_type, TEST_3);
            break;
        }
        case TEST_4:
        {
            ASSERT_EQ(event_type, TEST_4);
            break;
        }
        case TEST_5:
        {
            ASSERT_EQ(event_type, TEST_5);
            break;
        }
        case TEST_6:
        {
            ASSERT_EQ(event_type, TEST_6);
            break;
        }
        case TEST_7:
        {
            ASSERT_EQ(event_type, TEST_7);
            break;
        }
        case TEST_8:
        {
            ASSERT_EQ(event_type, TEST_8);
            break;
        }
        case TEST_9:
        {
            ASSERT_EQ(event_type, TEST_9);
            break;
        }
        default:
        {
            // 0 was already returned by GetNextEvent so the while loop quits before coming here
            ASSERT_EQ(event_type, TEST_NULL);
            break;
        }
        }

        eventidx++;
    }
}