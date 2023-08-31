//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"

class HotkeySystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        // Todo: Implement default settings for each test.
        // Add hotkeys in the queue. Set up event buffer.
        // Add mock implementation for Windows implementation of interface.

    }

};


TEST(HotkeyManagerSuite, Intialize) {
    // Todo: Implement, make sure to test all the initializer flags
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyManagerSuite, PollHotkeys) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyManagerSuite, AddHotkey) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyManagerSuite, RemoveHotkey) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyManagerSuite, SendHotkeyEvents) {
    // Todo: Implement, make sure to test sending one and multiple hotkeys
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyManagerSuite, GetEventBuffer) {
    // Todo: Implement, make sure to test if the buffer is usable and not empty.
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyInterfaceWindowsSuite, CheckHotkeys) {
    // Todo: Implement, test if no keys are pressed unless you can test for pressed keys in gtest somehow?
    ASSERT_EQ(0 ,1);
}

TEST(HotkeyInterfaceWindowsSuite, GetKeysPressed) {
    // Todo: Implement, test if no keys are pressed unless you can test for pressed keys in gtest somehow?
    ASSERT_EQ(0 ,1);
}
