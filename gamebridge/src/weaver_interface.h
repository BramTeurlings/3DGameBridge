#pragma once
#include "srloader.h"

class GameBridgeWeaver : public IGameBridgeManager
{
public:
    GameBridgeWeaver() = default;
    virtual void InitializeWeaver(SR::SRContext* sr_context) = 0;
};
