//
// Created by Bram on 31/08/2023.
//

#include <algorithm>
#include "gtest/gtest.h"
#include "hotkey_manager.h"
#include "hotkey_windows_impl.h"

class HotkeySystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
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

    ASSERT_NE(originalHotkeyImplementationPointer.get(), hotkeyManager.implementation.get()) << "Original hotkey implementation pointer shouldn't match the new hotkey implementation pointer";
}

TEST_F(HotkeySystemTests, PollHotkeys) {
    IHotkeys::CombinedHotkeyStrokes testCombo{};
    testCombo.separatedStrokes[0] = 0x11;
    testCombo.separatedStrokes[1] = 0x30;
    testCombo.separatedStrokes[2] = 0;
    testCombo.separatedStrokes[3] = 0;

    // Sort all keystrokes
    std::sort(&testCombo.separatedStrokes[0],&testCombo.separatedStrokes[4]);

    // Set a hotkey to true, we can verify that it will switch to false upon checking.
    hotkeyManager.implementation->hotkey_states.insert(std::make_pair(testCombo, true));

    // Poll the hotkeys, it should return true as at least one hotkey is registered.
    ASSERT_EQ(hotkeyManager.PollHotkeys(), 1) << "PollHotkeys() should return true when at least one hotkey is registered.";

    // Hotkeys are now polled. Check which buttons are pressed.
    ASSERT_EQ(hotkeyManager.implementation->hotkey_states[testCombo], 0) << "Hotkey state shouldn't be pressed.";

    // Clear the list of registered hotkeys
    hotkeyManager.registered_hotkeys.clear();

    // List of registered hotkeys is empty, the PollHotkeys() function should now return false;
    ASSERT_EQ(hotkeyManager.PollHotkeys(), 0) << "PollHotkeys() should return false when no hotkeys are registered.";
}

TEST_F(HotkeySystemTests, AddHotkey) {
    // First see if the amount of registered hotkeys is equal to 1.
    ASSERT_EQ(1, hotkeyManager.registered_hotkeys.size());

    // Add a new hotkey.
    hotkeyManager.AddHotkey(HotKeyEvent::GB_EVENT_HOTKEY_TOGGLE_LENS, 0x12, 0x31);

    // Assert that the hotkey was added.
    ASSERT_EQ(2, hotkeyManager.registered_hotkeys.size()) << "Hotkey was not added to registered_hotkeys after calling AddHotkey()";

    // Check all the separate strokes to see if they match.
    // The first two values are NULL because they are not filled in and sorted to the front in the AddHotkey() method.
    ASSERT_EQ(0x0, hotkeyManager.registered_hotkeys.back().hotkey_combination.separatedStrokes[0]) << "Keystroke [0] does not match expected keycode.";
    ASSERT_EQ(0x0, hotkeyManager.registered_hotkeys.back().hotkey_combination.separatedStrokes[1]) << "Keystroke [1] does not match expected keycode.";
    ASSERT_EQ(0x12, hotkeyManager.registered_hotkeys.back().hotkey_combination.separatedStrokes[2]) << "Keystroke [2] does not match expected keycode.";
    ASSERT_EQ(0x31, hotkeyManager.registered_hotkeys.back().hotkey_combination.separatedStrokes[3]) << "Keystroke [3] does not match expected keycode.";
}

TEST_F(HotkeySystemTests, RemoveHotkey) {
    // First see if the amount of registered hotkeys is equal to 1.
    ASSERT_EQ(1, hotkeyManager.registered_hotkeys.size()) << "No hotkeys initially registered.";

    // Test removing a hotkey that doesn't exist.
    hotkeyManager.RemoveHotkey(0x1, hotkeyManager.registered_hotkeys.begin()->hotkey_event);
    ASSERT_EQ(1, hotkeyManager.registered_hotkeys.size()) << "Removing a hotkey that isn't in the list shouldn't change registered_hotkey's size.";

    // Test removing a hotkey that does exist.
    hotkeyManager.RemoveHotkey(hotkeyManager.registered_hotkeys.begin()->hotkey_combination.combinedNumber, hotkeyManager.registered_hotkeys.begin()->hotkey_event);
    ASSERT_EQ(0, hotkeyManager.registered_hotkeys.size()) << "Removing a hotkey that exists en registered_hotkeys should decrement its size by one.";
}

TEST_F(HotkeySystemTests, SendHotkeyEvents) {
    // Todo: Implement, make sure to test sending one and multiple hotkeys
    ASSERT_EQ(0 ,1) << "Not implemented.";
}

TEST_F(HotkeySystemTests, GetEventBuffer) {
    // Todo: Implement, make sure to test if the buffer is usable and not empty.
    ASSERT_EQ(0 ,1) << "Not implemented.";
}

TEST_F(HotkeySystemTests, CheckHotkeys) {
    // Adds all registered hotkeys to the list of hotkeys.
    std::vector<IHotkeys::CombinedHotkeyStrokes> hotkeys;
    for(auto it = hotkeyManager.registered_hotkeys.begin(); it != hotkeyManager.registered_hotkeys.end(); it++) {
        hotkeys.push_back(it->hotkey_combination);
    }

    //Let the IHotkeys interface check the hotkeys, they can be retrieved from its "hotkey_states" member.
    std::map<IHotkeys::CombinedHotkeyStrokes, bool, IHotkeys::UnionComparator> checkedHotkeys = hotkeyManager.implementation->CheckHotkeys(hotkeys);

    ASSERT_EQ(checkedHotkeys.size(), 1) << "Checked hotkeys should only contain one hotkey.";
    ASSERT_EQ(checkedHotkeys.begin()->second, 0) << "Checked hotkey should not be pressed.";
}

TEST_F(HotkeySystemTests, GetKeysPressed) {
    // Test if no keys are pressed unless you can emulate pressed keys in gtest somehow?
    std::vector<uint32_t> pressedKeys;
    pressedKeys = hotkeyManager.implementation->GetKeysPressed();

    ASSERT_EQ(pressedKeys.size(), 0) << "No hotkeys should be pressed at this stage. Make sure no one is pressing hotkeys on the keyboard if one is attached.";
}
