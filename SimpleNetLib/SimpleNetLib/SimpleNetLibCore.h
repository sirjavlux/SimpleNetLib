#pragma once

#include "NetIncludes.h"

namespace Net
{
class SimpleNetLibCore
{
public:
    static SimpleNetLibCore* Initialize(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    static SimpleNetLibCore* Get();
    static void End();

    SimpleNetLibCore(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    ~SimpleNetLibCore();
    
private:
    static SimpleNetLibCore* instance_;
};
}