//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"

class EventSystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        // Todo: Implement default settings for each test.
        // Add an existing EventStream with an unprocessed event inside.

    }

};

TEST(EventManagerSuite, CreateEventStream) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(EventManagerSuite, GetEventStream) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(EventStreamWriterSuite, SubmitEvent) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(EventStreamWriterSuite, ClearStream) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(EventStreamReaderSuite, GetEventStream) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(EventStreamReaderSuite, GetNextEvent) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(EventStreamReaderSuite, ResetEventIndex) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}
