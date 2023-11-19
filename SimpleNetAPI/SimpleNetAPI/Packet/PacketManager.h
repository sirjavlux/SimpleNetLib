#pragma once

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
    
    void Initialize(const EPacketManagerType InPacketManagerType);
    PacketManager* Get();
    
    template<typename ComponentType>
    void RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const ComponentType&)>& InComponentHandleDelegateFunction);

private:
    template<typename ComponentType>
    bool IsPacketComponentValid();

    const EPacketManagerType type_;
    
    static PacketManager* instance_;
    
    PacketComponentHandleDelegator packetComponentHandleDelegator_;
    std::map<uint16_t, PacketComponentAssociatedData> packetComponentAssociatedData_;
};

template <typename ComponentType>
void PacketManager::RegisterPacketComponent(const EPacketHandlingType InHandlingType, const std::function<void(const ComponentType&)>& InComponentHandleDelegateFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    PacketComponentAssociatedData associatedData;
    associatedData.handlingType = InHandlingType;

    const ComponentType componentDefaultObject = ComponentType();
    packetComponentAssociatedData_.emplace({componentDefaultObject.GetIdentifier(), associatedData});

    packetComponentHandleDelegator_.MapComponentHandleDelegate<ComponentType>(InComponentHandleDelegateFunction);
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
