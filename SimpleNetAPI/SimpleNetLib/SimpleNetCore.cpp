#include "SimpleNetCore.h"

#include "Events/EventSystem.h"
#include "Packet/PacketManager.h"

SimpleNetCore* SimpleNetCore::instance_ = nullptr;

SimpleNetCore* SimpleNetCore::Initialize(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
{
    if (instance_ == nullptr)
    {
        instance_ = new SimpleNetCore(InPacketManagerType, InNetSettings);
    }
    
    return instance_;
}

SimpleNetCore* SimpleNetCore::Get()
{
    return instance_;
}

void SimpleNetCore::End()
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

SimpleNetCore::SimpleNetCore(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
{
    EventSystem::Initialize();
    PacketManager::Initialize(InPacketManagerType, InNetSettings);
}

SimpleNetCore::~SimpleNetCore()
{
    PacketManager::End();
    EventSystem::End();
}
