#include "PacketManager.h"

PacketManager* PacketManager::instance_ = nullptr;

PacketManager::PacketManager(const EPacketManagerType InPacketManagerType) : type_(InPacketManagerType)
{
    
}

PacketManager* PacketManager::Initialize(const EPacketManagerType InPacketManagerType)
{
    if (instance_ == nullptr)
    {
        instance_ = new PacketManager(InPacketManagerType);
    }
    
    return instance_;
}

PacketManager* PacketManager::Get()
{
    return instance_;
}
