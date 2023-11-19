#pragma once

#include "../NetIncludes.h"

class PacketComponent;

class PacketComponentHandleDelegator
{
public:
    void HandleComponent(const PacketComponent& InPacketComponent);

    template<typename ComponentType>
    void MapComponentHandleDelegate(const std::function<void(const ComponentType&)>& InFunction);

    template<typename OwningObject, typename ComponentType>
    void MapComponentHandleDelegateDynamic(OwningObject* InOwner, const std::function<void(OwningObject*, const ComponentType&)>& InFunction);
    
private:
    std::map<uint16_t, std::function<void(const PacketComponent&)>> delegates_;
};

template <typename ComponentType>
void PacketComponentHandleDelegator::MapComponentHandleDelegate(const std::function<void(const ComponentType&)>& InFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");
    
    // Use a lambda function as a wrapper
    auto wrapper = [InFunction](const PacketComponent& packetComponent) {
        InFunction(static_cast<const ComponentType&>(packetComponent));
    };

    const ComponentType componentDefaultObject = ComponentType();
    if (!componentDefaultObject.IsValid()) {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set.");
    }
    
    delegates_.emplace(componentDefaultObject.GetIdentifier(), wrapper);
}

template <typename OwningObject, typename ComponentType>
void PacketComponentHandleDelegator::MapComponentHandleDelegateDynamic(OwningObject* InOwner,
    const std::function<void(OwningObject*, const ComponentType&)>& InFunction)
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");

    // Use a lambda function as a wrapper
    const std::function<void(const PacketComponent&)>& function = 
        [InOwner, InFunction](const PacketComponent& packetComponent) {
            const ComponentType& component = static_cast<const ComponentType&>(packetComponent);
            InFunction(InOwner, component);
    };
    
    MapComponentHandleDelegate<ComponentType>(function);
}
