#include "srhelpers.h"

#include "easylogging++.h"
#include "instance.h"

SR::SRContext* XRGameBridge::CreateSrContext()
{
    if(!g_platform_manager)
    {
        SRPlatformManagerInitialize params{};
        params.game_bridge = g_game_bridge_instance;

        g_platform_manager = new PlatformManager(params);
    }

    while(!g_platform_manager->InitializeSRContext())
    {
        LOG(INFO) << "Failed creating SR context, retrying..";
    }
    return g_platform_manager->GetContext();
}
