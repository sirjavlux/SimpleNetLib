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

TEST(PacketTests, PacketCreation)
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
}

void TestFunction(const TestComponent& InPacketComponent)
{
    EXPECT_EQ(InPacketComponent.integerValue, 20);
    std::cout << InPacketComponent.integerValue << std::endl;
}

TEST(PacketDelegateTests, HandleDelegate)
{
    PacketComponentHandleDelegator delegator;
    TestComponent component;
    component.integerValue = 20;
    
    delegator.MapComponentHandleDelegate<TestComponent>(&TestFunction);

    delegator.HandleComponent(component);
}

class DynamicDelegateHandleClass
{
public:
    int testInt = 0;
    
    void TestFunction(const TestComponent& InPacketComponent)
    {
        EXPECT_EQ(InPacketComponent.integerValue, 20);
        EXPECT_EQ(testInt, 30);
        std::cout << InPacketComponent.integerValue << std::endl;
        std::cout << testInt << std::endl;
    }
};

TEST(PacketDelegateTests, DynamicHandleDelegate)
{
    PacketComponentHandleDelegator delegator;

    DynamicDelegateHandleClass delegateOwner;
    delegateOwner.testInt = 30;
    
    TestComponent component;
    component.integerValue = 20;
    
    delegator.MapComponentHandleDelegateDynamic<DynamicDelegateHandleClass, TestComponent>(&delegateOwner, &DynamicDelegateHandleClass::TestFunction);

    delegator.HandleComponent(component);
}