#pragma once

#include "PacketComponent.h"
#include "PacketTargetData.h"
#include "../Delegates/PacketComponentDelegator.h"
#include "../Network/NetHandler.h"

class ReturnAckComponent;

class NetHandler;
enum class EPacketHandlingType : uint8_t;

enum class ENetworkHandleType
{
    Server  = 0,
    Client  = 1,
};

namespace Net
{
class PacketManager
{
public:
    PacketManager(ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    ~PacketManager();
    
    static PacketManager* Initialize(ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    static PacketManager* Get();
    static void End();
    
    void Update();
    
    template<typename ComponentType>
    bool SendPacketComponent(const ComponentType& InPacketComponent, const sockaddr_storage& InTarget);

    template<typename ComponentType>
    bool SendPacketComponentMulticastOrParentConnection(const ComponentType& InPacketComponent);
    
    template <typename ComponentType>
    void RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData);

    template<typename ComponentType>
    void RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData, const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction);
    
    template<typename ComponentType, typename OwningObject>
    void RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData,
        const std::function<void(OwningObject*, const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObject* InOwnerObject);

    ENetworkHandleType GetManagerType() const { return managerType_; }

    template<typename ComponentType>
    const PacketComponentAssociatedData* FetchPacketComponentAssociatedData();
    const PacketComponentAssociatedData* FetchPacketComponentAssociatedData(uint16_t InIdentifier);

    void UpdateClientNetPosition(const sockaddr_storage& InAddress, const NetUtility::NetPosition& InPosition);
    
private:
    void HandleComponent(const sockaddr_storage& InComponentSender, const PacketComponent& InPacketComponent);
    
    void FixedUpdate();
    void UpdatePacketsWaitingForReturnAck(const sockaddr_storage& InTarget, const PacketTargetData& InTargetData) const;
    void UpdatePacketsToSend(const sockaddr_storage& InTarget, PacketTargetData& InTargetData) const;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    double updateLag_ = 0.0;

    bool DoesUpdateIterMatchPacketFrequency(const PacketFrequencyData& InPacketFrequencyData) const;
    
    static void OnNetTargetConnected(const sockaddr_storage& InTarget);
    static void OnNetTargetDisconnection(const sockaddr_storage& InTarget, ENetDisconnectType InDisconnectType);

    void OnAckReturnReceived(const sockaddr_storage& InNetTarget, const PacketComponent& InComponent);
    
    template<typename ComponentType>
    bool IsPacketComponentValid();

    template <typename ComponentType>
    void RegisterAssociatedData(const PacketComponentAssociatedData& InComponentSettings);

    void RegisterDefaultPacketComponents();

    void UpdateServerPinging();
    std::chrono::steady_clock::time_point lastTimePacketSent_;
    
    const ENetworkHandleType managerType_;
    
    static PacketManager* instance_;

    NetHandler* netHandler_ = nullptr;
    
    PacketComponentDelegator packetComponentHandleDelegator_;
    std::map<uint16_t, PacketComponentAssociatedData> packetAssociatedData_;

    std::map<sockaddr_storage, PacketTargetData> packetTargetDataMap_;
    
    const NetSettings netSettings_;

    int updateIterator_ = 0;
    
    friend class NetHandler;
};

template <typename ComponentType>
bool PacketManager::SendPacketComponent(const ComponentType& InPacketComponent, const sockaddr_storage& InTarget)
{
    lastTimePacketSent_ = std::chrono::steady_clock::now(); // Reset server ping timer
    
    // Check Component Validity
    if (!IsPacketComponentValid<ComponentType>())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }

    const PacketComponent& packetComponent = static_cast<const PacketComponent&>(InPacketComponent);

    if (packetTargetDataMap_.find(InTarget) == packetTargetDataMap_.end())
    {
        packetTargetDataMap_.insert({InTarget, PacketTargetData()});
    }
    PacketTargetData& packetTargetData = packetTargetDataMap_.at(InTarget);

    packetTargetData.AddPacketComponentToSend(std::make_shared<ComponentType>(InPacketComponent));
    
    return true;
}

template <typename ComponentType>
bool PacketManager::SendPacketComponentMulticastOrParentConnection(const ComponentType& InPacketComponent)
{
    // Server
    if (netHandler_->IsServer())
    {
        SendPacketComponent<ComponentType>(InPacketComponent, netHandler_->parentConnection_);
    }
    // Client
    else
    {
        // TODO: Implement packet net culling settings
        const std::vector<NetTarget> connections = netHandler_->connectionHandler_.GetCopy(); // TODO: Probably is better solution to this
        for (const NetTarget& netTarget : connections)
        {
            SendPacketComponent<ComponentType>(InPacketComponent, netTarget);
        }
    }

    return true;
}

template <typename ComponentType>
void PacketManager::RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InAssociatedData);
}

template <typename ComponentType>
void PacketManager::RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData, const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InAssociatedData);

    packetComponentHandleDelegator_.SubscribeToPacketComponentDelegate<ComponentType>(InFunction);
}

template<typename ComponentType, typename OwningObjectType>
void PacketManager::RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData, const std::function<void(OwningObjectType*, const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObjectType* InOwnerObject)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    if (InOwnerObject == nullptr)
    {
        throw std::runtime_error("Tried registering a packet component with a null owner object");
    }
    
    RegisterAssociatedData<ComponentType>(InAssociatedData);

    packetComponentHandleDelegator_.SubscribeToPacketComponentDelegate<ComponentType, OwningObjectType>(InFunction, InOwnerObject);
}

template <typename ComponentType>
const PacketComponentAssociatedData* PacketManager::FetchPacketComponentAssociatedData()
{
    const ComponentType componentDefaultObject = ComponentType();
    if (packetAssociatedData_.find(componentDefaultObject.GetIdentifier()) != packetAssociatedData_.end())
    {
        return &packetAssociatedData_.at(componentDefaultObject.GetIdentifier());
    }
    return nullptr;
}

template <typename ComponentType>
bool PacketManager::IsPacketComponentValid()
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    const ComponentType componentDefaultObject = ComponentType();
    if (packetAssociatedData_.find(componentDefaultObject.GetIdentifier()) == packetAssociatedData_.end()
        || !componentDefaultObject.IsValid())
    {
        return false;
    }

    return true;
}

template <typename ComponentType>
void PacketManager::RegisterAssociatedData(const PacketComponentAssociatedData& InComponentSettings)
{
    const ComponentType componentDefaultObject = ComponentType();
    if (!componentDefaultObject.IsValid())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }

    uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (packetAssociatedData_.find(identifier) != packetAssociatedData_.end())
    {
        throw std::runtime_error("Component with the same Identifier has already been registered!");
    }
    
    packetAssociatedData_.emplace(identifier, InComponentSettings);
}
}