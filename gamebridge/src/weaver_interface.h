#pragma once
#include <sr/management/srcontext.h>
#include "game_bridge_manager_interface.h"

class GameBridgeWeaver : public IGameBridgeManager
{
public:
    GameBridgeWeaver() = default;
    virtual void InitializeWeaver(SR::SRContext* sr_context) = 0;
};
