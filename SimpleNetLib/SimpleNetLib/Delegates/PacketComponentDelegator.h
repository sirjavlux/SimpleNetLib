#pragma once

#include "../NetIncludes.h"

namespace Net
{
class PacketComponent;

class PacketComponentDelegator
{
public:
    void HandleComponent(const sockaddr_storage& InNetTarget, const PacketComponent& InPacketComponent);
    
    template <typename ComponentType, typename OwningObject>
    void SubscribeToPacketComponentDelegate(
        const std::function<void(OwningObject*,const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObject* InOwner);

    template <typename ComponentType>
    void SubscribeToPacketComponentDelegate(
        const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction);

    template <typename ComponentType, typename OwningObject>
    void UnSubscribeFromPacketComponentDelegate(
        const std::function<void(OwningObject*,const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObject* InOwner);

    template <typename ComponentType>
    void UnSubscribeFromPacketComponentDelegate(
        const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction);
    
private:
    std::map<uint16_t, DynamicMulticastDelegate<const sockaddr_storage&, const PacketComponent&>> delegatesDynamic_;
    std::map<uint16_t, StaticMulticastDelegate<const sockaddr_storage&, const PacketComponent&>> delegatesStatic_;

    template <typename ComponentType>
    void CheckValidityOfPacket();
};

template <typename ComponentType, typename OwningObject>
void PacketComponentDelegator::SubscribeToPacketComponentDelegate(
    const std::function<void(OwningObject*, const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObject* InOwner)
{
    CheckValidityOfPacket<ComponentType>();

    const ComponentType componentDefaultObject = ComponentType();
    const uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (delegatesDynamic_.find(identifier) == delegatesDynamic_.end())
    {
        delegatesDynamic_.insert({ identifier, {} });
    }
    
    auto& delegateDynamic = delegatesDynamic_.at(identifier);
    delegateDynamic.AddDynamic<OwningObject>(InOwner, InFunction);
}

template <typename ComponentType>
void PacketComponentDelegator::SubscribeToPacketComponentDelegate(const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction)
{
    CheckValidityOfPacket<ComponentType>();

    const ComponentType componentDefaultObject = ComponentType();
    const uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (delegatesStatic_.find(identifier) == delegatesStatic_.end())
    {
        delegatesStatic_.insert({ identifier, {} });
    }
    
    auto& delegateStatic = delegatesStatic_.at(identifier);
    delegateStatic.Bind(InFunction);
}

template <typename ComponentType, typename OwningObject>
void PacketComponentDelegator::UnSubscribeFromPacketComponentDelegate(
    const std::function<void(OwningObject*, const sockaddr_storage&, const PacketComponent&)>& InFunction, OwningObject* InOwner)
{
    const ComponentType componentDefaultObject = ComponentType();
    const uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (delegatesDynamic_.find(identifier) != delegatesDynamic_.end())
    {
        auto& delegateDynamic = delegatesDynamic_.at(identifier);
        delegateDynamic.RemoveDynamic<OwningObject>(InOwner, InFunction);
    }
}

template <typename ComponentType>
void PacketComponentDelegator::UnSubscribeFromPacketComponentDelegate(const std::function<void(const sockaddr_storage&, const PacketComponent&)>& InFunction)
{
    const ComponentType componentDefaultObject = ComponentType();
    const uint16_t identifier = componentDefaultObject.GetIdentifier();
    if (delegatesStatic_.find(identifier) != delegatesStatic_.end())
    {
        auto& delegateStatic = delegatesStatic_.at(identifier);
        delegateStatic.UnBind(InFunction);
    }
}

template <typename ComponentType>
void PacketComponentDelegator::CheckValidityOfPacket()
{
    static_assert(std::is_base_of_v<PacketComponent, ComponentType>, "ComponentType must be derived from PacketComponent");

    const ComponentType componentDefaultObject = ComponentType();
    if (!componentDefaultObject.IsValid())
    {
        throw std::runtime_error("ComponentType isn't a valid PacketComponent. Make sure identifier and size is set.");
    }
}
}