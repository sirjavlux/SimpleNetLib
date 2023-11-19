#include "../SimpleNetAPI/Packet/Packet.h"
#include "../SimpleNetAPI/Packet/PacketComponent.h"
#include "../SimpleNetAPI/Packet/PacketComponentHandleDelegator.h"
#include "gtest/gtest.h"

class TestComponent : public PacketComponent
{
public:
    TestComponent() : PacketComponent(0, sizeof(TestComponent))
    { }
    
    int integerValue = 0;
};

void TestFunction(const TestComponent& InPacketComponent)
{
    std::cout << InPacketComponent.integerValue << std::endl;
}

TEST(PacketCreation, PacketTests)
{
    Packet packet = Packet(
        EPacketPacketType::ClientToServer, EPacketHandlingType::None);

    for (int i = 0; i < 20; ++i)
    {
        TestComponent testComponent;
        testComponent.integerValue = i;
        packet.AddComponent(testComponent);
    }

    std::vector<PacketComponent*> outComponents;
    packet.GetComponents(outComponents);

    int iter = 0;
    for (const PacketComponent* component : outComponents)
    {
        const TestComponent* castedComponent = reinterpret_cast<const TestComponent*>(component);
        
        EXPECT_TRUE(castedComponent != nullptr);
        EXPECT_EQ(iter, castedComponent->integerValue);

        ++iter;
    }

    PacketComponentHandleDelegator delegator;
    
    delegator.MapComponentHandleDelegate<TestComponent>(&TestFunction);

    for (const PacketComponent* component : outComponents)
    {
        delegator.HandleComponent(*component);
    }
}