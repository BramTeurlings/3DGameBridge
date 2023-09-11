//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"
#include "event_manager.h"

class EventSystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        // Todo: Implement default settings for each test.
        // Add an existing EventStream with an unprocessed event inside.
    }

    EventManager event_manager;
    EventStreamWriter<PlatformEvent> writer;
    //EventStreamReader reader;
};

TEST_F(EventSystemTests, CreateEventStream) {
    //writer = event_manager.CreateEventStream<PlatformEvent>(EventManagerType::SRGB_EVENT_MANAGER_TYPE_PLATFORM);

    ASSERT_EQ(1 ,1);
}

TEST_F(EventSystemTests, SubmitEvent) {

    //writer.SubmitEvent(PlatformEvent::SRGB_EVENT_PLATFORM_CONTEXT_INVALIDATED, 0, nullptr);

    ASSERT_EQ(1 ,1);
}

TEST_F(EventSystemTests, ClearStream) {
    // Todo: Implement
    ASSERT_EQ(1 ,1);
}

TEST_F(EventSystemTests, GetEventStream) {
    // Todo: Implement

    ASSERT_EQ(1 ,1);
}

TEST_F(EventSystemTests, GetNextEvent) {
    // Todo: Implement
    ASSERT_EQ(1 ,1);
}

TEST_F(EventSystemTests, ResetEventIndex) {
    // Todo: Implement
    ASSERT_EQ(1 ,1);
}
