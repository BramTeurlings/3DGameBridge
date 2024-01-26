//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"

class EventSystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        // Todo: Implement default settings for each test.
        // Set up a working SRContext and weaver already, make sure you're connected to a working event stream with an event already queued.
    }
};

TEST(PlatformManagerSuite, Initialize) {
    // Todo: Implement, make sure to test all the initializer flags
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerSuite, InitializeSRContext) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerSuite, InitializeWeaver) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerSuite, GetContext) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerSuite, GetWeaver) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerSuite, SwitchLens) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerSuite, ReinitializeContext) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

// DirectX 11 Tests
TEST(PlatformManagerDirectX11Suite, Initialize) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX11Suite, Weave) {
    // Todo: Implement, this might be very hard but it would be very useful for Dimenco as a whole and it's an important function to test
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX11Suite, SetLatency) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX11Suite, SetLatencyInFrames) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX11Suite, SetWeaving) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX11Suite, ReinitializeWeaver) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

// DirectX 12 Tests
TEST(PlatformManagerDirectX12Suite, Initialize) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX12Suite, Weave) {
    // Todo: Implement, this might be very hard but it would be very useful for Dimenco as a whole and it's an important function to test
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX12Suite, SetLatency) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX12Suite, SetLatencyInFrames) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX12Suite, SetWeaving) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(PlatformManagerDirectX12Suite, ReinitializeWeaver) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}
