//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"
#include "event_manager.h"

#define TEST_EVENT_COUNT 500

class EventSystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        platform_event_writer = event_manager.CreateEventStream(GB_EVENT_MANAGER_TYPE_PLATFORM, TEST_EVENT_COUNT, 0);
    }

    void FillEventStreams()
    {
        // Fill event streams
        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            platform_event_writer->SubmitEvent(GB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr);
        }

        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            hotkey_event_writer->SubmitEvent(GB_EVENT_HOTKEY_TOGGLE_LENS, 0, nullptr);
        }

        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            weaver_event_writer->SubmitEvent(GB_EVENT_WEAVER_WEAVING_ENABLED, 0, nullptr);
        }
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

TEST_F(EventSystemTests, CreateEventStream) {
    auto stream_writer = event_manager.CreateEventStream(GB_EVENT_MANAGER_TYPE_WEAVER, 300, 0);
    EventStream stream = stream_writer->GetEventStream();

    ASSERT_EQ(stream_writer->GetUsedBytes(), 0)
        << "Number of bytes in use should be 0";

    // Check if buffer was allocated
    ASSERT_NE(stream.buffer, nullptr)
        << "Buffer not allocated";

    // Check buffer size
    ASSERT_EQ(stream.buffer_size, sizeof(EventHeader) * (300 + 1))
        << "Unexpected buffer size";

    // Check user buffer size
    ASSERT_EQ(stream.user_buffer_size, sizeof(EventHeader) * 300)
        << "Unexpected buffer size";

    // Check stream id
    ASSERT_EQ(stream.stream_id, GB_EVENT_MANAGER_TYPE_WEAVER)
        << "Stream id should correspond to the manager type the stream was initialized with";

    // TODO test with extra data
}

TEST_F(EventSystemTests, GetEventStreamReader) {
    // Check platform event
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);
    ASSERT_TRUE(platform_event_reader);

    // Check if the object is valid by calling a function
    platform_event_reader->ResetEventIndexPointer();


    // Make sure other event streams are empty
    ASSERT_EQ(hotkey_event_reader, nullptr)
        << "hotkey_event_reader not empty, can't continue test";
    ASSERT_EQ(weaver_event_reader, nullptr)
        << "weaver_event_reader not empty, can't continue test";

    // Manager should not exist
    hotkey_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_HOTKEY);
    ASSERT_EQ(hotkey_event_reader, nullptr)
        << "Hotkey manager should be empty";

    // Check if the object is valid by calling a function
    platform_event_reader->ResetEventIndexPointer();
}

TEST_F(EventSystemTests, SubmitEvent) {
    // Fill event stream
    for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
        ASSERT_TRUE(platform_event_writer->SubmitEvent(GB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    // Check if 500 enums can be stored
    // Testing GetBytesUsed because it returns a value that's calculated by SubmitEvents
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * TEST_EVENT_COUNT)
        << "Buffer stored an unexpected amount of memory";

    // Check that no more than the maximum amount of events can be stored
    ASSERT_FALSE(platform_event_writer->SubmitEvent(GB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr))
        << "Event submitted that should have been rejected";

    // TODO test with extra data
}

TEST_F(EventSystemTests, GetUsedBytes)
{
    // Used byte should be empty
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), 0)
        << "Unexpected number of bytes in use, not 0 when the buffer should be empty";

    // Add a few events and check the amount of bytes used
    for (uint32_t i = 0; i < 5; i++) {
        platform_event_writer->SubmitEvent(GB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr);
    }

    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 5)
        << "Unexpected number of bytes in use";

    int64_t data = 50;
    // Test with a number for extra data
    for (uint32_t i = 0; i < 5; i++) {
        platform_event_writer->SubmitEvent(GB_EVENT_PLATFORM_CONTEXT_INVALIDATED, sizeof(data), &data);
    }

    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 5 + (sizeof(EventHeader) + sizeof(data)) * 5)
        << "Unexpected number of bytes in use";
}

TEST_F(EventSystemTests, GetNextEvent) {
    // Get event stream reader
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);

    // Write 499 mock events to stream
    for (uint32_t i = 0; i < TEST_EVENT_COUNT-1; i++) {
        int used_event = i % 3;

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
        }
    }

    // Write NULL_EVENT as the 500th event manually to stop reading at the end of the buffer
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
                ASSERT_TRUE(false)
                    << "Unknown event was added or data was changed, this line should never execute";
            }
        }

        eventidx++;
    }

    // Test last event. Since NULL_EVENT == 0 the while loop will not process it
    ASSERT_EQ(event_type, TEST_NULL)
        << "Event type not TEST_NULL";

    eventidx++; // Increment the last index
    ASSERT_EQ(eventidx, TEST_EVENT_COUNT)
        << "Unexpected event count";
}

TEST_F(EventSystemTests, ClearStream_StreamWriter) {
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);
    auto buffer = platform_event_writer->GetEventStream().buffer;

    // Pointer to first event in the stream
    EventHeader* header = reinterpret_cast<EventHeader*>(buffer.get());

    // Submit TEST_1 and get the event header
    for (int i = 0; i < 5; i++) {
        platform_event_writer->SubmitEvent(TEST_1);
    }

    // Make sure there is a size higher than 0
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 5)
        << "Unexpected number for Used bytes";

    // TEST_1 should be the first event in the stream
    ASSERT_EQ(header->event_type, TEST_1)
        << "Unexpected event, TEST_1 is not the first even in the stream";


    // Clear then submit TEST_2 with extra data number
    platform_event_writer->ClearStream();

    for (int i = 0; i < 5; i++) {
        platform_event_writer->SubmitEvent(TEST_2);
    }

    // Size should be exactly the same as with TEST_1
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), sizeof(EventHeader) * 5)
        << "Bytes used should be 0";

    // TEST_2 should be the first event in the stream
    ASSERT_EQ(header->event_type, TEST_2)
        << "Unexpected event, TEST_2 is not the first even in the stream";

    // TODO test with extra data
}

TEST_F(EventSystemTests, ClearStream_StreamReader) {
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);

    // Test ClearStream for a EventStreamReader
    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;

    for (int i = 0; i < 5; i++) {
        platform_event_writer->SubmitEvent(TEST_1);
    }

    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_1) << "Event not equal to TEST_1";
    }

    platform_event_writer->ClearStream();
    platform_event_reader->ResetEventIndexPointer();

    // Make sure TEST_NULL is the only event returned after clearing
    platform_event_writer->ClearStream();
    platform_event_reader->ResetEventIndexPointer();
    // Test 3 times to make sure the index pointer is not moving
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL) << "Event not equal to TEST_NULL";
}

TEST_F(EventSystemTests, ResetEventIndexPointer) {
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);

    // Fill buffer
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

    // Test all events in buffer
    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;
    for (int i = 1; i <= 9; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "Event not equal to " << i;
    }
    // Final event should be TEST_NULL
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL)
        << "Final event should be TEST_NULL";

    // Reset the pointer and test all events again
    platform_event_reader->ResetEventIndexPointer();
    for (int i = 1; i <= 9; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "Event not equal to " << i;
    }
    // Final event should be TEST_NULL
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL)
    << "Final event should be TEST_NULL";
}

TEST_F(EventSystemTests, PrepareForEventStreamSubmission)
{
    // Get event stream reader
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);

    // Make sure all streams are cleared and reader pointers are reset
    event_manager.PrepareForEventStreamSubmission();

    // Pointer to first event in the stream
    EventHeader* header = reinterpret_cast<EventHeader*>(platform_event_writer->GetEventStream().buffer.get());

    // Stream should be cleared
    ASSERT_EQ(platform_event_writer->GetUsedBytes(), 0)
        << "Unexpected number for Used bytes";

    // GB_EVENT_NULL should be the first event in the stream
    ASSERT_EQ(header->event_type, GB_EVENT_NULL)
        << "Unexpected event, TEST_1 is not the first even in the stream";
}

TEST_F(EventSystemTests, PrepareForEventStreamProcessing)
{
    platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);

    // Fill buffer
    platform_event_writer->SubmitEvent(TEST_1);
    platform_event_writer->SubmitEvent(TEST_2);
    platform_event_writer->SubmitEvent(TEST_3);
    platform_event_writer->SubmitEvent(TEST_4);
    platform_event_writer->SubmitEvent(TEST_5);
    platform_event_writer->SubmitEvent(TEST_6);
    platform_event_writer->SubmitEvent(TEST_7);
    platform_event_writer->SubmitEvent(TEST_8);
    platform_event_writer->SubmitEvent(TEST_9);

    // Test all events in buffer
    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;
    for (int i = 1; i <= 9; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "Event not equal to " << i;
    }

    // Resets the pointer and adds EVENT_NULL
    event_manager.PrepareForEventStreamProcessing();

    // Can process events again
    for (int i = 1; i <= 9; i++) {
        ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), i)
            << "Event not equal to " << i;
    }
    // Final event should be TEST_NULL
    ASSERT_EQ(platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data), TEST_NULL)
        << "Final event should be TEST_NULL";
}
// Basically a full system test that also uses PrepareForEventStreamSubmission and PrepareForEventStreamProcessing
TEST_F(EventSystemTests, FullTest)
{
    for (int redo = 0; redo < 5; redo++) {
        // Get event stream reader
        platform_event_reader = event_manager.GetEventStreamReader(GB_EVENT_MANAGER_TYPE_PLATFORM);

        // Make sure all streams are cleared and reader pointers are reset
        event_manager.PrepareForEventStreamSubmission();

        // Pointer to first event in the stream
        EventHeader* header = reinterpret_cast<EventHeader*>(platform_event_writer->GetEventStream().buffer.get());

        // Stream should be cleared
        ASSERT_EQ(platform_event_writer->GetUsedBytes(), 0)
            << "Unexpected number for Used bytes";

        // GB_EVENT_NULL should be the first event in the stream
        ASSERT_EQ(header->event_type, GB_EVENT_NULL)
            << "Unexpected event, TEST_1 is not the first even in the stream";

        // Write 500 mock events to stream
        // Try 50 more the make sure only 500 are written, and the other 50 did not write to the stream.

        for (uint32_t i = 0; i < TEST_EVENT_COUNT + 50; i++) {
            int used_event = i % 9;

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

        // Check whether PrepareForEventStreamProcessing
        // Make sure every buffer has a NULL_EVENT at the end and writer pointers are reset
        event_manager.PrepareForEventStreamProcessing();

        int index = 1;

        // Test reading all events as expected
        uint32_t event_type;
        size_t extra_data_size = 0;
        void* event_data = nullptr;
        while (platform_event_reader->GetNextEvent(event_type, extra_data_size, event_data)) {
            switch (event_type) {
            case TEST_1:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_2:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_3:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_4:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_5:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_6:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_7:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_8:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            case TEST_9:
            {
                ASSERT_EQ(event_type, index);
                break;
            }
            default:
            {
                // 0 was already returned by GetNextEvent so the while loop quits before coming here
                ASSERT_EQ(event_type, TEST_NULL);
                break;
            }
            }

            index = index % 9 + 1;
        }
    }
}
