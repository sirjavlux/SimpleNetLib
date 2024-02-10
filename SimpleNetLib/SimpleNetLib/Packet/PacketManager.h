#pragma once

#include "../NetIncludes.h"

#include "Packet.h"
#include "PacketComponent.h"
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

class PacketTargetData
{
public:
    explicit PacketTargetData()
        : regularPacket_(EPacketHandlingType::None),
        ackPacket_(EPacketHandlingType::Ack)
    {
        regularPacket_.Reset();
        ackPacket_.Reset();
    }

    Packet& GetPacketByHandlingType(const EPacketHandlingType InHandlingType)
    {
        switch (InHandlingType)
        {
        case EPacketHandlingType::None:
            return regularPacket_;
        case EPacketHandlingType::Ack:
            return ackPacket_;
        default: ;
        }

        throw std::runtime_error("Invalid EPacketHandlingType was used");
    }

    void PushAckPacketIfContainingData()
    {
        if (ackPacket_.IsEmpty())
        {
            return;
        }
        
        ackPacketsNotReturned_.insert({ ackPacket_.GetIdentifier(), ackPacket_ });
        ackPacket_.Reset();

        std::cout << "Added ack packet " << ackPacket_.GetIdentifier() << "\n";
    }
    
    const std::map<int32_t, Packet>& GetPacketsNotReturned() const { return ackPacketsNotReturned_; }
    void RemoveReturnedPacket(const int32_t InIdentifier)
    {
        ackPacketsNotReturned_.erase(InIdentifier);
    }
    
private:
    Packet regularPacket_;
    Packet ackPacket_;

    std::map<int32_t, Packet> ackPacketsNotReturned_;
};

class PacketManager
{
public:
    PacketManager(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    ~PacketManager();
    
    static PacketManager* Initialize(const ENetworkHandleType InPacketManagerType, const NetSettings& InNetSettings);
    static PacketManager* Get();
    static void End();
    
    void Update();
    
    template<typename ComponentType>
    bool SendPacketComponent(const ComponentType& InPacketComponent, const NetTarget& InTarget);
    
    template <class ComponentType>
    void RegisterPacketComponent(EPacketHandlingType InHandlingType);

    template<typename ComponentType>
    void RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const NetTarget&, const PacketComponent&)>& InFunction);
    
    template<typename ComponentType, typename OwningObject>
    void RegisterPacketComponent(EPacketHandlingType InHandlingType,
        const std::function<void(OwningObject*, const NetTarget&, const PacketComponent&)>& InFunction, OwningObject* InOwnerObject);

    ENetworkHandleType GetManagerType() const { return managerType_; }

    void HandleComponent(const NetTarget& InNetTarget, const PacketComponent& InPacketComponent);
    
private:
    void FixedUpdate(); // TODO: Implement this
    std::chrono::steady_clock::time_point lastUpdateTime_;
    double updateLag_ = 0.0;

    static void OnNetTargetConnected(const NetTarget& InTarget);
    static void OnNetTargetDisconnection(const NetTarget& InTarget, const ENetDisconnectType InDisconnectType);

    void OnAckReturnReceived(const NetTarget& InNetTarget, const PacketComponent& InComponent);
    
    template<typename ComponentType>
    bool IsPacketComponentValid();

    template <typename ComponentType>
    void RegisterAssociatedData(const EPacketHandlingType InHandlingType);

    void RegisterDefaultPacketComponents();
    
    const ENetworkHandleType managerType_;
    
    static PacketManager* instance_;

    NetHandler* netHandler_ = nullptr;
    
    PacketComponentDelegator packetComponentHandleDelegator_;
    std::map<uint16_t, PacketComponentAssociatedData> packetComponentAssociatedData_;

    std::map<NetTarget, PacketTargetData> packetTargetDataMap_;
    
    const NetSettings netSettings_;
    
    friend class NetHandler;
};

template <typename ComponentType>
bool PacketManager::SendPacketComponent(const ComponentType& InPacketComponent, const NetTarget& InTarget)
{
    // Check Component Validity
    if (!IsPacketComponentValid<ComponentType>())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }

    const PacketComponent& packetComponent = static_cast<const PacketComponent&>(InPacketComponent);
    const PacketComponentAssociatedData& packetComponentAssociatedData = packetComponentAssociatedData_.find(packetComponent.GetIdentifier())->second;

    if (packetTargetDataMap_.find(InTarget) == packetTargetDataMap_.end())
    {
        packetTargetDataMap_.insert({InTarget, PacketTargetData()});
    }
    PacketTargetData& packetTargetData = packetTargetDataMap_.at(InTarget);

    Packet& relevantPacket = packetTargetData.GetPacketByHandlingType(packetComponentAssociatedData.handlingType);
    
    if (relevantPacket.AddComponent(packetComponent) == EAddComponentResult::SizeOutOfBounds)
    {
        if (relevantPacket.GetPacketType() == EPacketHandlingType::Ack)
        {
            packetTargetData.PushAckPacketIfContainingData();
        }
        else
        {
            netHandler_->SendPacketToTargetAndResetPacket(InTarget, relevantPacket);
        }
        
        // Try adding component again
        relevantPacket.AddComponent(packetComponent);
    }
    
    return true;
}

template <typename ComponentType>
void PacketManager::RegisterPacketComponent(const EPacketHandlingType InHandlingType)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InHandlingType);
}

template <typename ComponentType>
void PacketManager::RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const NetTarget&, const PacketComponent&)>& InFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InHandlingType);

    packetComponentHandleDelegator_.SubscribeToPacketComponentDelegate<ComponentType>(InFunction);
}

template<typename ComponentType, typename OwningObjectType>
void PacketManager::RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(OwningObjectType*, const NetTarget&, const PacketComponent&)>& InFunction, OwningObjectType* InOwnerObject)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    if (InOwnerObject == nullptr)
    {
        throw std::runtime_error("Tried registering a packet component with a null owner object");
    }
    
    RegisterAssociatedData<ComponentType>(InHandlingType);

    packetComponentHandleDelegator_.SubscribeToPacketComponentDelegate<ComponentType, OwningObjectType>(InFunction, InOwnerObject);
}

template <typename ComponentType>
bool PacketManager::IsPacketComponentValid()
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    const ComponentType componentDefaultObject = ComponentType();
    if (packetComponentAssociatedData_.find(componentDefaultObject.GetIdentifier()) == packetComponentAssociatedData_.end()
        || !componentDefaultObject.IsValid())
    {
        return false;
    }

    return true;
}

template <typename ComponentType>
void PacketManager::RegisterAssociatedData(const EPacketHandlingType InHandlingType)
{
    if (InHandlingType != EPacketHandlingType::Ack && InHandlingType != EPacketHandlingType::None)
    {
        throw std::runtime_error("InHandlingType isn't a valid EPacketHandlingType. Needs to be EPacketHandlingType::Ack or EPacketHandlingType::None");
    }
    
    const ComponentType componentDefaultObject = ComponentType();
    if (!componentDefaultObject.IsValid())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }
    
    PacketComponentAssociatedData associatedData;
    associatedData.handlingType = InHandlingType;

    uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (packetComponentAssociatedData_.find(identifier) != packetComponentAssociatedData_.end())
    {
        throw std::runtime_error("Component with the same Identifier has already been registered!");
    }
    
    packetComponentAssociatedData_.emplace(identifier, associatedData);
}
