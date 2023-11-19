#pragma once

#include "Packet.h"
#include "../NetIncludes.h"
#include "PacketComponentHandleDelegator.h"

enum class EPacketHandlingType : uint8_t;

enum class EPacketManagerType
{
    Server  = 0,
    Client  = 1,
};

struct PacketComponentAssociatedData
{
    EPacketHandlingType handlingType;
};

class PacketManager
{
public:
    PacketManager(const EPacketManagerType InPacketManagerType);

    static PacketManager* Initialize(const EPacketManagerType InPacketManagerType);
    static PacketManager* Get();

    template<typename ComponentType>
    bool SendPacketComponent(const ComponentType& InPacketComponent);
    
    template<typename ComponentType>
    void RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const ComponentType&)>& InComponentHandleDelegateFunction);
    
    template<typename ComponentType, typename OwningObject>
    void RegisterPacketComponent(EPacketHandlingType InHandlingType,
        const std::function<void(OwningObject*, const ComponentType&)>& InFunction, OwningObject* InOwnerObject);

private:
    template<typename ComponentType>
    bool IsPacketComponentValid();

    template <typename ComponentType>
    void RegisterAssociatedData(const EPacketHandlingType InHandlingType);
    
    const EPacketManagerType type_;
    
    static PacketManager* instance_;
    
    PacketComponentHandleDelegator packetComponentHandleDelegator_;
    std::map<uint16_t, PacketComponentAssociatedData> packetComponentAssociatedData_;
};

template <typename ComponentType>
bool PacketManager::SendPacketComponent(const ComponentType& InPacketComponent)
{
    // Check Component Validity
    if (!IsPacketComponentValid<ComponentType>())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }

    // Code...


    
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
