﻿#pragma once

#include <chrono>

#include "Packet.h"
#include "PacketComponent.h"
#include "NetStructs.hpp"
#include "PacketComponentHandleDelegator.h"
#include "NetHandler.h"

enum class EPacketHandlingType : uint8_t;

enum class EPacketManagerType
{
    Server  = 0,
    Client  = 1,
};

inline bool operator<(const sockaddr& InLhs, const sockaddr& InRhs)
{
    // Compare based on the address family (IPv4 or IPv6)
    if (InLhs.sa_family == AF_INET) {
        const sockaddr_in& lhs = reinterpret_cast<const sockaddr_in&>(InLhs);
        const sockaddr_in& rhs = reinterpret_cast<const sockaddr_in&>(InRhs);

        // Compare IPv4 addresses
        if (lhs.sin_addr.s_addr < rhs.sin_addr.s_addr) {
            return true;
        } 
        if (lhs.sin_addr.s_addr == rhs.sin_addr.s_addr)
        {
            // Compare port numbers
            return ntohs(lhs.sin_port) < ntohs(rhs.sin_port);
        }
    }

    return false;
}

class PacketTargetData
{
public:
    explicit PacketTargetData()
        : regularPacket_(EPacketHandlingType::None),
        ackPacket_(EPacketHandlingType::Ack),
        ackReturnPacket_(EPacketHandlingType::AckReturn)
    { }

    Packet& GetPacketByHandlingType(const EPacketHandlingType InHandlingType)
    {
        switch (InHandlingType)
        {
        case EPacketHandlingType::None:
            return regularPacket_;
        case EPacketHandlingType::Ack:
            return ackPacket_;
        case EPacketHandlingType::AckReturn:
            return ackReturnPacket_;
        }

        throw std::runtime_error("Invalid EPacketHandlingType was used");
    }

private:
    Packet regularPacket_;
    Packet ackPacket_;
    Packet ackReturnPacket_;
};

class PacketManager
{
public:
    PacketManager(const EPacketManagerType InPacketManagerType, const NetSettings& InNetSettings);

    static PacketManager* Initialize(const EPacketManagerType InPacketManagerType, const NetSettings& InNetSettings);
    static PacketManager* Get();

    void Update();
    
    template<typename ComponentType>
    bool SendPacketComponent(const ComponentType& InPacketComponent, const sockaddr& InNetTarget);
    
    template<typename ComponentType>
    void RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const ComponentType&)>& InComponentHandleDelegateFunction);
    
    template<typename ComponentType, typename OwningObject>
    void RegisterPacketComponent(EPacketHandlingType InHandlingType,
        const std::function<void(OwningObject*, const ComponentType&)>& InFunction, OwningObject* InOwnerObject);

    EPacketManagerType GetManagerType() const { return managerType_; }
    
private:
    void FixedUpdate(); // TODO: Implement this
    std::chrono::steady_clock::time_point lastUpdateTime_;
    double updateLag_ = 0.0;

    template<typename ComponentType>
    bool IsPacketComponentValid();

    template <typename ComponentType>
    void RegisterAssociatedData(const EPacketHandlingType InHandlingType);
    
    const EPacketManagerType managerType_;
    
    static PacketManager* instance_;

    NetHandler netHandler_;
    
    PacketComponentHandleDelegator packetComponentHandleDelegator_;
    std::map<uint16_t, PacketComponentAssociatedData> packetComponentAssociatedData_;

    std::map<sockaddr, PacketTargetData> packetTargetDataMap_;
};

template <typename ComponentType>
bool PacketManager::SendPacketComponent(const ComponentType& InPacketComponent, const sockaddr& InNetTarget)
{
    // Check Component Validity
    if (!IsPacketComponentValid<ComponentType>())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }

    const PacketComponent& packetComponent = static_cast<const PacketComponent&>(InPacketComponent);
    const PacketComponentAssociatedData& packetComponentAssociatedData = packetComponentAssociatedData_.find(packetComponent.GetIdentifier())->second;

    if (packetTargetDataMap_.find(InNetTarget) == packetTargetDataMap_.end())
    {
        packetTargetDataMap_.emplace(InNetTarget, PacketTargetData());
    }
    PacketTargetData& packetTargetData = packetTargetDataMap_.at(InNetTarget);

    Packet& relevantPacket = packetTargetData.GetPacketByHandlingType(packetComponentAssociatedData.handlingType);

    if (relevantPacket.AddComponent(packetComponent) == EAddComponentResult::SizeOutOfBounds)
    {
        // TODO: Pre-send packet and clear...

        // Try adding component again
        relevantPacket.AddComponent(packetComponent);
    }
    
    return true;
}

template <typename ComponentType>
void PacketManager::RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const ComponentType&)>& InComponentHandleDelegateFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InHandlingType);

    packetComponentHandleDelegator_.MapComponentHandleDelegate<ComponentType>(InComponentHandleDelegateFunction);
}

template<typename ComponentType, typename OwningObject>
void PacketManager::RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(OwningObject*, const ComponentType&)>& InFunction, OwningObject* InOwnerObject)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");

    RegisterAssociatedData<ComponentType>(InHandlingType);

    packetComponentHandleDelegator_.MapComponentHandleDelegateDynamic(InFunction, InOwnerObject);
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
    
    packetComponentAssociatedData_.emplace(componentDefaultObject.GetIdentifier(), associatedData);
}