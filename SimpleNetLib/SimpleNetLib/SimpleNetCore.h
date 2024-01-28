#pragma once

#include "NetIncludes.h"

class SimpleNetCore
{
public:
    static SimpleNetCore* Initialize(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    static SimpleNetCore* Get();
    static void End();

    SimpleNetCore(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    ~SimpleNetCore();
    
private:
    static SimpleNetCore* instance_;
};
