#include "PacketManager.h"

PacketManager* PacketManager::instance_ = nullptr;

PacketManager::PacketManager(const EPacketManagerType InPacketManagerType) : type_(InPacketManagerType)
{
    
}

void PacketManager::Initialize(const EPacketManagerType InPacketManagerType)
{
    if (instance_ == nullptr)
    {
        instance_ = new PacketManager(InPacketManagerType);
    }
}

PacketManager* PacketManager::Get()
{
    return instance_;
}
