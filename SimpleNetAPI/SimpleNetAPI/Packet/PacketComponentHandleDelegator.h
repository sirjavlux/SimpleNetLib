#pragma once

#include "../Utility/NetTag.h"

class PacketComponent;

class PacketComponentHandleDelegator
{
public:
    void HandleComponent(const PacketComponent& InPacketComponent);

    template<typename ComponentType>
    void MapComponentHandleDelegate(const std::function<void(const ComponentType&)>& InFunction);
    
private:
    std::map<uint16_t, std::function<void(const PacketComponent&)>> delegates_;
};

template <typename ComponentType>
void PacketComponentHandleDelegator::MapComponentHandleDelegate(const std::function<void(const ComponentType&)>& InFunction)
{
    if (std::is_base_of<PacketComponent, ComponentType>::value)
    {
        // Use a lambda function as a wrapper with the correct signature
        auto wrapper = [InFunction](const PacketComponent& packetComponent) {
            InFunction(static_cast<const ComponentType&>(packetComponent));
        }; 
        
        const ComponentType component = ComponentType();
        delegates_.insert({component.GetIdentifier(), wrapper});
    }
}
