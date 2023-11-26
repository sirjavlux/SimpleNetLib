#include "PacketManager.h"

#include "../Network/NetHandler.h"

PacketManager* PacketManager::instance_ = nullptr;

PacketManager::PacketManager(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
    : managerType_(InPacketManagerType), netHandler_(nullptr)
{
}

PacketManager::~PacketManager()
{
    delete netHandler_;
}

PacketManager* PacketManager::Initialize(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
{
    if (instance_ == nullptr)
    {
        instance_ = new PacketManager(InPacketManagerType, InNetSettings);
        instance_->netHandler_ = new NetHandler(InNetSettings);
    }
    
    return instance_;
}

PacketManager* PacketManager::Get()
{
    return instance_;
}

void PacketManager::End()
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

void PacketManager::Update()
{
    using namespace std::chrono;

    const steady_clock::time_point currentTime = steady_clock::now();
    const duration<double> deltaTime = duration_cast<duration<double>>(currentTime - lastUpdateTime_);
    lastUpdateTime_ = currentTime;

    updateLag_ += deltaTime.count();

    if (updateLag_ >= FIXED_UPDATE_TIME)
    {
        FixedUpdate();
        updateLag_ -= FIXED_UPDATE_TIME;
    }
}

void PacketManager::FixedUpdate()
{
    
}

void PacketManager::OnNetTargetConnected(const NetTarget& InTarget)
{
    
}

void PacketManager::OnNetTargetDisconnection(const NetTarget& InTarget, const ENetDisconnectType InDisconnectType)
{
    
}
