//
// Created by Bram on 31/08/2023.
//

#include "gtest/gtest.h"

class GameBridgeSystemTests : public ::testing::Test {

protected:
    virtual void SetUp() {
        // Todo: Implement default settings for each test.
        // Register some starting managers and make sure event_manager is initialized.

    }

};

TEST(GameBridgeSuite, Initialize) {
    // Todo: Implement, make sure to test all the initializer flags
    ASSERT_EQ(0 ,1);
}

TEST(GameBridgeSuite, RegisterManager) {
    // Todo: Implement, make sure to try every type of manager
    ASSERT_EQ(0 ,1);
}

TEST(GameBridgeSuite, GetEventManager) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}

TEST(GameBridgeSuite, DeregisterManager) {
    // Todo: Implement
    ASSERT_EQ(0 ,1);
}
