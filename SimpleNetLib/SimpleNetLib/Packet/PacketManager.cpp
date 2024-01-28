#include "PacketManager.h"

#include "../Events/EventSystem.h"
#include "../Network/NetHandler.h"
#include "CorePacketComponents/ServerConnect.hpp"
#include "CorePacketComponents/ServerDisconnect.hpp"

PacketManager* PacketManager::instance_ = nullptr;

PacketManager::PacketManager(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
    : managerType_(InPacketManagerType), netHandler_(nullptr), netSettings_(InNetSettings)
{
    RegisterDefaultPacketComponents();
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
    EventSystem::Get()->onClientConnectEvent.Execute(InTarget);
}

void PacketManager::OnNetTargetDisconnection(const NetTarget& InTarget, const ENetDisconnectType InDisconnectType)
{
    EventSystem::Get()->onClientDisconnectEvent.Execute(InTarget, InDisconnectType);
}

void PacketManager::RegisterDefaultPacketComponents()
{
    RegisterPacketComponent<ServerConnectPacketComponent, NetHandler>(EPacketHandlingType::Ack, &NetHandler::OnChildConnectionReceived, netHandler_);
    RegisterPacketComponent<ServerDisconnectPacketComponent, NetHandler>(EPacketHandlingType::Ack, &NetHandler::OnChildDisconnectReceived, netHandler_);
}
