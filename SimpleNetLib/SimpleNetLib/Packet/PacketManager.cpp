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

const PacketComponentAssociatedData* PacketManager::FetchPacketComponentAssociatedData(const uint16_t InIdentifier)
{
    if (packetAssociatedData_.find(InIdentifier) != packetAssociatedData_.end())
    {
        return &packetAssociatedData_.at(InIdentifier);
    }
    return nullptr;
}

void PacketManager::UpdateClientNetPosition(const sockaddr_storage& InAddress, const NetUtility::NetPosition& InPosition)
{
    netHandler_->GetNetConnectionHandler().UpdateNetCullingPosition(InAddress, InPosition);
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

        UpdatePacketsWaitingForReturnAck(target, targetData);
        UpdatePacketsToSend(target, targetData);
    }

    ++updateIterator_;
    if (updateIterator_ > UPDATE_ITERATOR_RESET_NUMBER)
    {
        updateIterator_ = 0;
    }
}

void PacketManager::UpdatePacketsWaitingForReturnAck(const sockaddr_storage& InTarget, const PacketTargetData& InTargetData) const
{
    const std::map<uint32_t, std::pair<PacketFrequencyData, Packet>>& packetsNotReturned = InTargetData.GetPacketsNotReturned();
    for (const std::pair<uint32_t, std::pair<PacketFrequencyData, Packet>>& packetDataPair : packetsNotReturned)
    {
        const PacketFrequencyData& frequencyData = packetDataPair.second.first;
        if (DoesUpdateIterMatchPacketFrequency(frequencyData))
        {
            const Packet& packet = packetDataPair.second.second;
            netHandler_->SendPacketToTarget(InTarget, packet);
        }
    }
}

void PacketManager::UpdatePacketsToSend(const sockaddr_storage& InTarget, PacketTargetData& InTargetData) const
{
    std::map<PacketFrequencyData, std::vector<std::shared_ptr<PacketComponent>>>& packetComponents = InTargetData.GetPacketComponentsToSend();
    std::vector<PacketFrequencyData> toRemove;
    for (auto packetIter = packetComponents.begin(); packetIter != packetComponents.end(); ++packetIter)
    {
        const PacketFrequencyData& frequencyData = packetIter->first;
        if (DoesUpdateIterMatchPacketFrequency(frequencyData))
        {
            const EPacketHandlingType handlingType = frequencyData.handlingType;
            Packet packet = Packet(handlingType);
            
            std::vector<std::shared_ptr<PacketComponent>>& componentContainer = packetIter->second;

            // Add Components to new Packet
            int componentsAdded = 0;
            if (!componentContainer.empty())
            {
                std::reverse(componentContainer.begin(), componentContainer.end());
                for (auto it = componentContainer.begin(); it != componentContainer.end(); ++it)
                {
                    const EAddComponentResult result = packet.AddComponent(**it);
                    if (result != EAddComponentResult::Success)
                    {
                        break;
                    }

                    ++componentsAdded;
                }
            }
            
            // Remove added components
            if (componentsAdded > 0)
            {
                componentContainer.erase(componentContainer.begin(), componentContainer.begin() + componentsAdded);
                std::reverse(componentContainer.begin(), componentContainer.end());
            }

            // Add to ack container if of Ack type
            if (handlingType == EPacketHandlingType::Ack)
            {
                InTargetData.PushAckPacketIfContainingData(frequencyData, packet);
            }
            
            if (componentContainer.empty())
            {
                toRemove.push_back(frequencyData);
            }
                
            netHandler_->SendPacketToTarget(InTarget, packet);
        }
    }

    // Remove empty frequency containers
    for (const PacketFrequencyData& dataToRemove : toRemove)
    {
        packetComponents.erase(dataToRemove);
    }
}

bool PacketManager::DoesUpdateIterMatchPacketFrequency(const PacketFrequencyData& InPacketFrequencyData) const
{
    if (InPacketFrequencyData.frequency == 0)
    {
        return false;
    }

    const int updateIterator = std::abs(updateIterator_);
    return updateIterator % InPacketFrequencyData.frequency == 0;
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
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData{
            false,
            0.f,
            EPacketHandlingType::Ack,
            1.4f
        };
        RegisterPacketComponent<ServerConnectPacketComponent, NetHandler>(associatedData, &NetHandler::OnChildConnectionReceived, netHandler_);
    }
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData{
            false,
            0.f,
            EPacketHandlingType::Ack,
            1.4f
        };
        RegisterPacketComponent<ServerDisconnectPacketComponent, NetHandler>(associatedData, &NetHandler::OnChildDisconnectReceived, netHandler_);
    }
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData{
            false,
            0.3f,
            EPacketHandlingType::None
        };
        RegisterPacketComponent<ReturnAckComponent, PacketManager>(associatedData, &PacketManager::OnAckReturnReceived, this);
    }
    {
        const PacketComponentAssociatedData associatedData = PacketComponentAssociatedData
        {
            false,
            1.f,
            EPacketHandlingType::None
        };
        RegisterPacketComponent<ServerPingPacketComponent>(associatedData);
    }
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