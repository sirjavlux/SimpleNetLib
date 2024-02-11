#include "SimpleNetLibCore.h"

#include "Events/EventSystem.h"
#include "Packet/PacketManager.h"

namespace Net
{
SimpleNetLibCore* SimpleNetLibCore::instance_ = nullptr;

SimpleNetLibCore* SimpleNetLibCore::Initialize(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
{
    if (instance_ == nullptr)
    {
        instance_ = new SimpleNetLibCore(InPacketManagerType, InNetSettings);
    }
    
    return instance_;
}

SimpleNetLibCore* SimpleNetLibCore::Get()
{
    return instance_;
}

void SimpleNetLibCore::End()
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

SimpleNetLibCore::SimpleNetLibCore(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
{
    EventSystem::Initialize();
    PacketManager::Initialize(InPacketManagerType, InNetSettings);
}

SimpleNetLibCore::~SimpleNetLibCore()
{
    PacketManager::End();
    EventSystem::End();
}
}