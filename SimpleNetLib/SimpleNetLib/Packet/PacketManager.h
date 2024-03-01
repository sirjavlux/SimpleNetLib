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
    bool SendPacketComponentToParent(const ComponentType& InPacketComponent);
    
    template<typename ComponentType>
    bool SendPacketComponent(const ComponentType& InPacketComponent, const sockaddr_storage& InTarget);

    // Keep in mind, this can be a bit expensive due to no lod or culling
    template<typename ComponentType>
    bool SendPacketComponentMulticast(const ComponentType& InPacketComponent);
    
    template<typename ComponentType>
    bool SendPacketComponentMulticastWithLod(const ComponentType& InPacketComponent, const NetUtility::NetVector3& InPosition);
    
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
    
    void UpdateClientNetPosition(const sockaddr_storage& InAddress, const NetUtility::NetVector3& InPosition);
    
    PacketComponentDelegator& GetPacketComponentDelegator() { return packetComponentHandleDelegator_; }

    float GetDeltaTime() const { return lastDeltaTime_; }

    bool IsServer() const { return managerType_ == ENetworkHandleType::Server; }
    
private: 
    void HandleComponent(const sockaddr_storage& InComponentSender, const PacketComponent& InPacketComponent);
    
    void FixedUpdate();
    void UpdatePacketsWaitingForReturnAck(const sockaddr_storage& InTarget, const PacketTargetData& InTargetData) const;
    void UpdatePacketsToSend(const sockaddr_storage& InTarget, PacketTargetData& InTargetData) const;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    double updateLag_ = 0.0;

    std::chrono::steady_clock::time_point lastUpdateFinishedTime_;
    
    bool DoesUpdateIterMatchPacketFrequency(const PacketFrequencyData& InPacketFrequencyData) const;
    
    static void OnNetTargetConnected(const sockaddr_storage& InTarget);
    static void OnNetTargetDisconnection(const sockaddr_storage& InTarget, ENetDisconnectType InDisconnectType);

    void OnAckReturnReceived(const sockaddr_storage& InTarget, const PacketComponent& InComponent);
    
    template<typename ComponentType>
    bool IsPacketComponentValid();

    template <typename ComponentType>
    void RegisterAssociatedData(PacketComponentAssociatedData InComponentSettings);

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

    float lastDeltaTime_ = 0.f;
    
    friend class NetHandler;
};

template <typename ComponentType>
bool PacketManager::SendPacketComponentToParent(const ComponentType& InPacketComponent)
{
    if (netHandler_->IsServer())
    {
        return false;
    }

    return SendPacketComponent<ComponentType>(InPacketComponent, netHandler_->parentConnection_);
}

template <typename ComponentType>
bool PacketManager::SendPacketComponent(const ComponentType& InPacketComponent, const sockaddr_storage& InTarget)
{
    lastTimePacketSent_ = std::chrono::steady_clock::now(); // Reset server ping timer
    
    // Check Component Validity
    if (!IsPacketComponentValid<ComponentType>())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }
    
    PacketTargetData& packetTargetData = packetTargetDataMap_.at(InTarget);

    packetTargetData.AddPacketComponentToSend(std::make_shared<ComponentType>(InPacketComponent));
    
    return true;
}

template <typename ComponentType>
bool PacketManager::SendPacketComponentMulticast(const ComponentType& InPacketComponent)
{
    // Can't multicast upstream from client to server
    if (netHandler_->IsServer())
    {
        const std::unordered_map<sockaddr_in, NetTarget>& connections = netHandler_->connectionHandler_.GetConnections();
        for (const auto& connection : connections)
        {
            SendPacketComponent(InPacketComponent, NetUtility::RetrieveStorageFromIPv4Address(connection.first));
        }
        return true;
    }
    return false;
}

template <typename ComponentType>
bool PacketManager::SendPacketComponentMulticastWithLod(const ComponentType& InPacketComponent, const NetUtility::NetVector3& InPosition)
{
    // Can't multicast upstream from client to server
    if (netHandler_->IsServer())
    {
        const std::unordered_map<sockaddr_in, NetTarget>& connections = netHandler_->connectionHandler_.GetConnections();
        for (const auto& connection : connections)
        {
            // Check associated data validity
            const PacketComponentAssociatedData* associatedData = FetchPacketComponentAssociatedData(InPacketComponent.GetIdentifier());
            if (!associatedData)
            {
                continue;
            }
            
            // Preliminary culling check
            const float distanceSqr = InPosition.DistanceSqr(connection.second.netCullingPosition);
            if (associatedData->distanceToCullPacketComponentAt > -1
                && distanceSqr > std::powf(associatedData->distanceToCullPacketComponentAt, 2.f))
            {
                continue; // Continue if culling distance is exceeded
            }

            const sockaddr_storage storageAddress = NetUtility::RetrieveStorageFromIPv4Address(connection.first);
            PacketTargetData& packetTargetData = packetTargetDataMap_.at(storageAddress);
            
            // Send packet with the correct lod frequency
            packetTargetData.AddPacketComponentToSendWithLod(std::make_shared<ComponentType>(InPacketComponent), distanceSqr);
        }
        return true;
    }
    return false;
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
void PacketManager::RegisterAssociatedData(PacketComponentAssociatedData InComponentSettings)
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

    InComponentSettings.SortLodFrequencies();
    
    packetAssociatedData_.emplace(identifier, InComponentSettings);
}
}