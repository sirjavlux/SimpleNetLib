#include "PacketManager.h"

#include "../Events/EventSystem.h"
#include "../Network/NetHandler.h"
#include "CorePacketComponents/ReturnAckComponent.hpp"
#include "CorePacketComponents/ServerConnect.hpp"
#include "CorePacketComponents/ServerDisconnect.hpp"
#include "CorePacketComponents/ServerPing.hpp"

namespace Net
{
PacketManager* PacketManager::instance_ = nullptr;

PacketManager::PacketManager(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings)
    : managerType_(InPacketManagerType), netHandler_(nullptr), netSettings_(InNetSettings)
{
    lastTimePacketSent_ = std::chrono::steady_clock::now();
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
        
        instance_->RegisterDefaultPacketComponents();
        instance_->netHandler_->Initialize();
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

void PacketManager::HandleComponent(const sockaddr_storage& InComponentSender, const PacketComponent& InPacketComponent)
{
    packetComponentHandleDelegator_.HandleComponent(InComponentSender, InPacketComponent); 
}

void PacketManager::FixedUpdate()
{
    netHandler_->Update();
    
    if (!netHandler_->IsServer())
    {
        UpdateServerPinging();
    }
    
    for (std::pair<const sockaddr_storage, PacketTargetData>& packetTargetPair : packetTargetDataMap_)
    {
        const sockaddr_storage& target = packetTargetPair.first;
        PacketTargetData& targetData = packetTargetPair.second;
        
        // Regular Packets
        if (Packet& packet = targetData.GetPacketByHandlingType(EPacketHandlingType::None); !packet.IsEmpty())
        {
            netHandler_->SendPacketToTargetAndResetPacket(target, packet);
        }

        // Packets not returned
        targetData.PushAckPacketIfContainingData();
        const std::map<int32_t, Packet>& packetsNotReturned = targetData.GetPacketsNotReturned();
        for (const std::pair<const int32_t, Packet>& packetIdentifierPair : packetsNotReturned)
        {
            netHandler_->SendPacketToTarget(target, packetIdentifierPair.second);
        }
    }
}

void PacketManager::OnNetTargetConnected(const sockaddr_storage& InTarget)
{
    EventSystem::Get()->onClientConnectEvent.Execute(InTarget);
}

void PacketManager::OnNetTargetDisconnection(const sockaddr_storage& InTarget, const ENetDisconnectType InDisconnectType)
{
    EventSystem::Get()->onClientDisconnectEvent.Execute(InTarget, InDisconnectType);
}

void PacketManager::OnAckReturnReceived(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent)
{
    const ReturnAckComponent* ackComponent = static_cast<const ReturnAckComponent*>(&InComponent);
    packetTargetDataMap_.at(InNetTarget).RemoveReturnedPacket(ackComponent->ackIdentifier);
}

void PacketManager::RegisterDefaultPacketComponents()
{
    RegisterPacketComponent<ServerConnectPacketComponent, NetHandler>(EPacketHandlingType::Ack, &NetHandler::OnChildConnectionReceived, netHandler_);
    RegisterPacketComponent<ServerDisconnectPacketComponent, NetHandler>(EPacketHandlingType::Ack, &NetHandler::OnChildDisconnectReceived, netHandler_);
    RegisterPacketComponent<ReturnAckComponent, PacketManager>(EPacketHandlingType::None, &PacketManager::OnAckReturnReceived, this);
    RegisterPacketComponent<ServerPingPacketComponent>(EPacketHandlingType::Ack);
}

void PacketManager::UpdateServerPinging()
{
    using namespace std::chrono;
    
    const steady_clock::time_point currentTime = steady_clock::now();
    const duration<double> deltaTime = duration_cast<duration<double>>(currentTime - lastTimePacketSent_);
    if (deltaTime.count() > NET_TIME_UNTIL_NEXT_SERVER_PING)
    {
        const ServerPingPacketComponent serverPingPacketComponent;
        SendPacketComponent(serverPingPacketComponent, netHandler_->parentConnection_);
    }
}
}