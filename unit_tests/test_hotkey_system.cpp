//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"
#include "hotkey_manager.h"
#include "hotkey_windows_impl.cpp"

class HotkeySystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        // Todo: Implement default settings for each test.
        // Add hotkeys in the queue. Set up event buffer.
        // Add mock implementation for Windows implementation of interface.

        // Create event manager
        EventManager eventManager;

        // Create GameBridge instance:
        GameBridge gameBridge(eventManager);

        // Create Windows hotkey implementation
        auto windowsHotkeys = std::make_shared<WindowsHotkeyImplementation>();

        HotkeyManagerInitializeFlags flags = HotkeyManagerInitializeFlags::SRGB_HOTKEY_MANAGER_NO_FLAGS;

        HotkeyManagerInitialize init{windowsHotkeys, flags, gameBridge};

        hotkeyManager = HotkeyManager(init);

        // Add some hotkeys to poll
        // Hotkeys are comprised of 8 bits per keystroke. We support any hotkey combination as long as it is shorter than 4 keys.
        // Hotkeys are split into each individual keystroke by use of a union with uint8_t strokes[4].
        hotkeyManager.AddHotkey(HotKeyEvent::GB_EVENT_HOTKEY_TOGGLE_LENS, 0x11, 0x30);
    }

    HotkeyManager hotkeyManager;

};


TEST_F(HotkeySystemTests, Intialize) {
    std::shared_ptr<IHotkeys> originalHotkeyImplementationPointer = hotkeyManager.implementation;

    // The code below should be a copy of the SetUp method's initialization method.
    // Create event manager
    EventManager eventManager;

    // Create GameBridge instance:
    GameBridge gameBridge(eventManager);

    // Create Windows hotkey implementation
    auto windowsHotkeys = std::make_shared<WindowsHotkeyImplementation>();

    // Make sure to test all the initializer flags
    HotkeyManagerInitializeFlags flags = HotkeyManagerInitializeFlags::SRGB_HOTKEY_MANAGER_NO_FLAGS;

    HotkeyManagerInitialize init{windowsHotkeys, flags, gameBridge};

    hotkeyManager = HotkeyManager(init);

    ASSERT_NE(originalHotkeyImplementationPointer.get(), hotkeyManager.implementation.get());
}

TEST_F(HotkeySystemTests, PollHotkeys) {
    IHotkeys::HotkeyCombination testCombo;
    testCombo.separatedStrokes[0] = 0x11;
    testCombo.separatedStrokes[1] = 0x30;
    testCombo.separatedStrokes[2] = 0;
    testCombo.separatedStrokes[3] = 0;

    // Sort all keystrokes
    std::sort(&testCombo.separatedStrokes[0],&testCombo.separatedStrokes[4]);

    // Set a hotkey to true, we can verify that it will switch to false upon checking.
    hotkeyManager.implementation->hotkey_states.insert(std::make_pair(testCombo, true));

    // Check the hotkeys to fill
    if(hotkeyManager.PollHotkeys()){
        // Hotkeys are now polled. Check which buttons are pressed.
        ASSERT_EQ(hotkeyManager.implementation->hotkey_states[testCombo], 0);
    } else {
        // Polling of hotkeys failed.
        ASSERT_EQ("Polling of hotkeys has failed" ,"");
    }
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
