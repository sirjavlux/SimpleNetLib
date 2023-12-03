#include "../SimpleNetLib/SimpleNetCore.h"
#include "../SimpleNetLib/Packet/Packet.h"
#include "../SimpleNetLib/Packet/PacketComponent.h"
#include "../SimpleNetLib/Delegates/PacketComponentHandleDelegator.h"
#include "../SimpleNetLib/Events/EventSystem.h"
#include "../SimpleNetLib/Packet/PacketManager.h"
#include "gtest/gtest.h"

class TestComponent : public PacketComponent
{
public:
    TestComponent() : PacketComponent(0, sizeof(TestComponent))
    { }
    
    int integerValue = 0;
};

class InvalidComponent : public PacketComponent
{
public:
    InvalidComponent() : PacketComponent(0, 0)
    { }
};

TEST(PacketTests, PacketCreation)
{
    Packet packet = Packet(EPacketHandlingType::None);

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

void TestComponentFunction(const NetTarget& InNetTarget, const TestComponent& InPacketComponent)
{
    EXPECT_EQ(InPacketComponent.integerValue, 20);
    std::cout << InPacketComponent.integerValue << std::endl;
}

void InvalidComponentFunction(const NetTarget& InNetTarget, const InvalidComponent& InPacketComponent)
{
    
}

TEST(PacketTests, PacketManagerComponentRegestring)
{
    const NetSettings settings; // Left empty acting as server with no parent server
    PacketManager* packetManager = PacketManager::Initialize(ENetworkHandleType::Server, settings);
    
    TestComponent validComponent;
    InvalidComponent invalidComponent;

    bool failed = false;
    try
    {
        packetManager->RegisterPacketComponent<InvalidComponent>(EPacketHandlingType::None, &InvalidComponentFunction);
    }
    catch (...)
    {
        failed = true;
    }

    EXPECT_TRUE(failed);

    failed = false;
    try
    {
        packetManager->RegisterPacketComponent<TestComponent>(EPacketHandlingType::None, &TestComponentFunction);
    }
    catch (...)
    {
        failed = true;
    }

    EXPECT_FALSE(failed);
    
    PacketManager::End();
}

TEST(PacketDelegateTests, HandleDelegate)
{
    PacketComponentHandleDelegator delegator;
    TestComponent component;
    component.integerValue = 20;
    
    delegator.MapComponentHandleDelegate<TestComponent>(&TestComponentFunction);

    delegator.HandleComponent(NetTarget(), component);
}

class DynamicDelegateHandleClass
{
public:
    int testInt = 0;
    
    void TestComponentFunction(const NetTarget& InNetTarget, const TestComponent& InPacketComponent)
    {
        EXPECT_EQ(InPacketComponent.integerValue, 20);
        EXPECT_EQ(testInt, 30);
        std::cout << InPacketComponent.integerValue << std::endl;
        std::cout << testInt << std::endl;
    }

    void OnConnectEvent(const NetTarget& InNetTarget)
    {
        
    }
};

TEST(PacketDelegateTests, DynamicHandleDelegate)
{
    const NetSettings settings; // Left empty acting as server with no parent server
    SimpleNetCore* netCore = SimpleNetCore::Initialize(ENetworkHandleType::Client, settings);
    
    PacketComponentHandleDelegator delegator;

    DynamicDelegateHandleClass delegateOwner;
    delegateOwner.testInt = 30;
    
    TestComponent component;
    component.integerValue = 20;
    
    delegator.MapComponentHandleDelegateDynamic<TestComponent, DynamicDelegateHandleClass>(&DynamicDelegateHandleClass::TestComponentFunction, &delegateOwner);

    delegator.HandleComponent(NetTarget(), component);

    EventSystem::Get()->onClientConnectEvent.AddDynamic<DynamicDelegateHandleClass>(std::make_shared<DynamicDelegateHandleClass>(delegateOwner), &DynamicDelegateHandleClass::OnConnectEvent);
    
    PacketManager::End();
}

TEST(PacketTests, SendPacket)
{
    const NetSettings settings; // Left empty acting as server with no parent server
    PacketManager* packetManager = PacketManager::Initialize(ENetworkHandleType::Client, settings);

    packetManager->RegisterPacketComponent<TestComponent>(EPacketHandlingType::None, &TestComponentFunction);
    
    for (int i = 0; i < 100000; ++i)
    {
        TestComponent testComponent;
        testComponent.integerValue = i;

        //sockaddr testAddress;
        //testAddress.sa_family = AF_INET;
        
        //EXPECT_TRUE(packetManager->SendPacketComponent<TestComponent>(testComponent, testAddress));
    }
    
    PacketManager::End();
}