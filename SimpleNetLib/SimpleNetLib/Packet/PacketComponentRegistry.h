#pragma once

#include "PacketComponent.h"
#include "PacketTargetData.h"
#include "../SimpleNetLibCore.h"
#include "../Delegates/PacketComponentDelegator.h"

namespace Net
{
class PacketComponentRegistry
{
public:
    template <typename ComponentType>
    void RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData);

    template<typename ComponentType>
    void RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData, const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction);
    
    template<typename ComponentType, typename OwningObject>
    void RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData,
        const std::function<void(OwningObject*, const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObject* InOwnerObject);

    template<typename ComponentType>
    bool IsPacketComponentValid();

    template<typename ComponentType>
    const PacketComponentAssociatedData* FetchPacketComponentAssociatedData();
    const PacketComponentAssociatedData* FetchPacketComponentAssociatedData(uint16_t InIdentifier);
    
private:
    template <typename ComponentType>
    void RegisterAssociatedData(PacketComponentAssociatedData InComponentSettings);

    std::unordered_map<uint16_t, PacketComponentAssociatedData> packetAssociatedData_;
};

template <typename ComponentType>
void PacketComponentRegistry::RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InAssociatedData);
}

template <typename ComponentType>
void PacketComponentRegistry::RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData, const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    RegisterAssociatedData<ComponentType>(InAssociatedData);

    SimpleNetLibCore::Get()->GetPacketComponentDelegator().SubscribeToPacketComponentDelegate<ComponentType>(InFunction);
}

template<typename ComponentType, typename OwningObjectType>
void PacketComponentRegistry::RegisterPacketComponent(const PacketComponentAssociatedData& InAssociatedData, const std::function<void(OwningObjectType*, const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObjectType* InOwnerObject)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    if (InOwnerObject == nullptr)
    {
        throw std::runtime_error("Tried registering a packet component with a null owner object");
    }
    
    RegisterAssociatedData<ComponentType>(InAssociatedData);

    SimpleNetLibCore::Get()->GetPacketComponentDelegator()->SubscribeToPacketComponentDelegate<ComponentType, OwningObjectType>(InFunction, InOwnerObject);
}

template <typename ComponentType>
const PacketComponentAssociatedData* PacketComponentRegistry::FetchPacketComponentAssociatedData()
{
    const ComponentType componentDefaultObject = ComponentType();
    const uint16_t identifier = componentDefaultObject.GetIdentifier();
    
    return packetAssociatedData_.find(identifier) != packetAssociatedData_.end()
        ? packetAssociatedData_.at(identifier) : nullptr;
}

template <typename ComponentType>
bool PacketComponentRegistry::IsPacketComponentValid()
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
void PacketComponentRegistry::RegisterAssociatedData(PacketComponentAssociatedData InComponentSettings)
{
    const ComponentType componentDefaultObject = ComponentType();
    if (!componentDefaultObject.IsValid())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set");
    }

    const uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (packetAssociatedData_.find(identifier) != packetAssociatedData_.end())
    {
        return;
    }

    InComponentSettings.SortLodFrequencies();
    
    packetAssociatedData_[identifier] = InComponentSettings;
}
}