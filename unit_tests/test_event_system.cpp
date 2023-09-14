//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"
#include "event_manager.h"
#include "../../../Program Files/Simulated Reality/SDK/third_party/OpenCV/include/opencv2/core/hal/interface.h"

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
            platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr);
        }

        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            hotkey_event_writer.SubmitEvent(GB_EVENT_HOTKEY_TOGGLE_LENS, 0, nullptr);
        }

        for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
            weaver_event_writer.SubmitEvent(SRGB_EVENT_WEAVER_WEAVING_ENABLED, 0, nullptr);
        }
    }

    void SetUpEvenStreamReaders()
    {
        ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_HOTKEY, hotkey_event_reader));
        ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_WEAVER, weaver_event_reader));
        ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM, platform_event_reader));
    }

    EventManager event_manager;

    EventStreamWriter platform_event_writer;
    EventStreamWriter hotkey_event_writer;
    EventStreamWriter weaver_event_writer;

    EventStreamReader platform_event_reader;
    EventStreamReader hotkey_event_reader;
    EventStreamReader weaver_event_reader;
};

TEST_F(EventSystemTests, GetWriterEventStream) {
    EventStream stream = platform_event_writer.GetEventStream();
    ASSERT_NE(stream.buffer, nullptr);
}

TEST_F(EventSystemTests, EventSystem) {

    // Check platform writer
    EventStream stream = platform_event_writer.GetEventStream();
    ASSERT_GE(stream.buffer_size, 1);
    ASSERT_GE(stream.max_event_count, 1);

    // Check hotkey writer
    stream = hotkey_event_writer.GetEventStream();
    ASSERT_GE(stream.buffer_size, 1);
    ASSERT_GE(stream.max_event_count, 1);

    // Check weaver writer
    stream = weaver_event_writer.GetEventStream();
    ASSERT_GE(stream.buffer_size, 1);
    ASSERT_GE(stream.max_event_count, 1);
}

TEST_F(EventSystemTests, GetUsedBytes)
{
    ASSERT_EQ(platform_event_writer.GetUsedBytes(), 0);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_FALSE(platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer.GetUsedBytes(), sizeof(EventHeader) * 5);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_FALSE(platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer.GetUsedBytes(), sizeof(EventHeader) * 10);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_FALSE(platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer.GetUsedBytes(), sizeof(EventHeader) * 15);

    for (uint32_t i = 0; i < 5; i++) {
        ASSERT_FALSE(platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    ASSERT_EQ(platform_event_writer.GetUsedBytes(), sizeof(EventHeader) * 20);
}

TEST_F(EventSystemTests, SubmitEvent) {
    // Fill event stream
    for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
        ASSERT_TRUE(platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr));
    }

    // Check if 500 enums can be stored
    EventStream stream = platform_event_writer.GetEventStream();
    ASSERT_EQ(platform_event_writer.GetUsedBytes(), sizeof(EventHeader) * TEST_EVENT_COUNT);
    ASSERT_EQ(stream.max_event_count, TEST_EVENT_COUNT)
    << "500 events should be stored";

    ASSERT_FALSE(platform_event_writer.SubmitEvent(SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr))
    << "Check if not more than 500 enums can be stored, SubmitEvent should return false";

    // TODO test with extra data
}


// TODO test after EventStreamWriter tests and use a writer inside this test if that makes sense
TEST_F(EventSystemTests, ClearStream) {
    FillEventStreams();

    EventStream stream = platform_event_writer.GetEventStream();

    platform_event_writer.ClearStream();
    ASSERT_EQ(platform_event_writer.GetUsedBytes(), 0)
    << "Platform events bytes in use should be 0";

    hotkey_event_writer.ClearStream();
    ASSERT_EQ(hotkey_event_writer.GetUsedBytes(), 0)
    << "Hot key events max used bytes not 0";

    weaver_event_writer.ClearStream();
    ASSERT_EQ(weaver_event_writer.GetUsedBytes(), 0)
    << "Weaver events max used bytes not 0";
}

TEST_F(EventSystemTests, GetEventStreamReader) {
    ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_HOTKEY, hotkey_event_reader));
    ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_WEAVER, weaver_event_reader));
    ASSERT_TRUE(event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM, platform_event_reader));
}

TEST_F(EventSystemTests, GetNextEvent) {
    // Create mock event types
    enum TestEnums
    {
        TEST_NULL,
        TEST_1,
        TEST_2,
        TEST_3
    };

    // Write mock events to stream
    for (uint32_t i = 0; i < TEST_EVENT_COUNT; i++) {
        int used_event = i % 3;

        switch (used_event)
        {
        case 0:
            platform_event_writer.SubmitEvent(TEST_1, 0, nullptr);
        case 1:
            platform_event_writer.SubmitEvent(TEST_2, 0, nullptr);
        case 2:
            platform_event_writer.SubmitEvent(TEST_3, 0, nullptr);
        }
    }

    // Get event stream reader
    event_manager.GetEventStreamReader(SRGB_EVENT_MANAGER_TYPE_PLATFORM, platform_event_reader);

    // Read events from stream
    uint32_t eventidx = 0;

    uint32_t event_type;
    size_t extra_data_size = 0;
    void* event_data = nullptr;
    while (platform_event_reader.GetNextEvent(event_type, extra_data_size, event_data)) {
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
        }
        default:
        {
            ASSERT_EQ(event_type, TEST_NULL);
            ASSERT_EQ(eventidx, TEST_EVENT_COUNT);
            break;
        }
        }

        eventidx++;
    }
}

TEST_F(EventSystemTests, ResetEventIndex) {
    // Todo: Implement
    ASSERT_EQ(1 ,1);
}
