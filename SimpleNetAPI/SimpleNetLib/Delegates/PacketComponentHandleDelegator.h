#pragma once

#include "../NetIncludes.h"

class PacketComponent;

class PacketComponentHandleDelegator
{
public:
    void HandleComponent(const NetTarget& InNetTarget, const PacketComponent& InPacketComponent);

    template<typename ComponentType>
    void MapComponentHandleDelegate(const std::function<void(const NetTarget&, const ComponentType&)>& InFunction);

    template <typename ComponentType, typename OwningObject>
    void MapComponentHandleDelegateDynamic(const std::function<void(OwningObject*,const NetTarget&,  const ComponentType&)>& InFunction, OwningObject* InOwner);
    
private:
    std::map<uint16_t, std::function<void(const NetTarget&, const PacketComponent&)>> delegates_;
};

template <typename ComponentType>
void PacketComponentHandleDelegator::MapComponentHandleDelegate(const std::function<void(const NetTarget&, const ComponentType&)>& InFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    // Use a lambda function as a wrapper
    auto wrapper = [InFunction](const NetTarget& netTarget, const PacketComponent& packetComponent) {
        InFunction(netTarget, static_cast<const ComponentType&>(packetComponent));
    };

    const ComponentType componentDefaultObject = ComponentType();
    if (!componentDefaultObject.IsValid())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set.");
    }
    
    delegates_.emplace(componentDefaultObject.GetIdentifier(), wrapper);
}

template <typename ComponentType, typename OwningObject>
void PacketComponentHandleDelegator::MapComponentHandleDelegateDynamic(const std::function<void(OwningObject*, const NetTarget&, const ComponentType&)>& InFunction, OwningObject* InOwner)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");

    // Use a lambda function as a wrapper
    const std::function<void(const NetTarget&, const PacketComponent&)>& function = 
        [InOwner, InFunction](const NetTarget& netTarget, const PacketComponent& packetComponent) {
            const ComponentType& component = static_cast<const ComponentType&>(packetComponent);
            InFunction(InOwner, netTarget, component);
    };
    
    MapComponentHandleDelegate<ComponentType>(function);
}
