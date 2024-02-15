#include "gtest/gtest.h"

#include "../SimpleNetLib/SimpleNetLibCore.h"
#include "../SimpleNetLib/Packet/Packet.h"
#include "../SimpleNetLib/Packet/PacketComponent.h"
#include "../SimpleNetLib/Delegates/PacketComponentDelegator.h"
#include "../SimpleNetLib/Events/EventSystem.h"
#include "../SimpleNetLib/Packet/PacketManager.h"

using namespace Net;

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

    std::vector<const PacketComponent*> outComponents;
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

void TestComponentFunction(const sockaddr_storage& InNetTarget, const PacketComponent& InPacketComponent)
{
    const TestComponent* testComponent = static_cast<const TestComponent*>(&InPacketComponent);
    EXPECT_EQ(testComponent->integerValue, 20);
    std::cout << testComponent->integerValue << std::endl;
}

void InvalidComponentFunction(const sockaddr_storage& InNetTarget, const PacketComponent& InPacketComponent)
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
        packetManager->RegisterPacketComponent<InvalidComponent>({}, &InvalidComponentFunction);
    }
    catch (...)
    {
        failed = true;
    }

    EXPECT_TRUE(failed);

    failed = false;
    try
    {
        packetManager->RegisterPacketComponent<TestComponent>({}, &TestComponentFunction);
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
    PacketComponentDelegator delegator;
    TestComponent component;
    component.integerValue = 20;
    
    delegator.SubscribeToPacketComponentDelegate<TestComponent>(&TestComponentFunction);

    delegator.HandleComponent(sockaddr_storage(), component);
}

class DynamicDelegateHandleClass
{
public:
    int testInt = 0;
    
    void TestComponentFunction(const sockaddr_storage& InNetTarget, const PacketComponent& InPacketComponent)
    {
        const TestComponent* testComponent = static_cast<const TestComponent*>(&InPacketComponent);
        EXPECT_EQ(testComponent->integerValue, 20);
        EXPECT_EQ(testInt, 30);
        std::cout << testComponent->integerValue << std::endl;
        std::cout << testInt << std::endl;
    }

    void OnConnectEvent(const sockaddr_storage& InNetTarget)
    {
        
    }
};

TEST(PacketDelegateTests, DynamicHandleDelegate)
{
    const NetSettings settings; // Left empty acting as server with no parent server
    SimpleNetLibCore* netCore = SimpleNetLibCore::Initialize(ENetworkHandleType::Client, settings);
    
    PacketComponentDelegator delegator;

    DynamicDelegateHandleClass delegateOwner;
    delegateOwner.testInt = 30;
    
    TestComponent component;
    component.integerValue = 20;
    
    delegator.SubscribeToPacketComponentDelegate<TestComponent, DynamicDelegateHandleClass>(&DynamicDelegateHandleClass::TestComponentFunction, &delegateOwner);

    delegator.HandleComponent(sockaddr_storage(), component);

    EventSystem::Get()->onClientConnectEvent.AddDynamic<DynamicDelegateHandleClass>(&delegateOwner, &DynamicDelegateHandleClass::OnConnectEvent);
    
    PacketManager::End();
}

TEST(PacketTests, SendPacket)
{
    const NetSettings settings; // Left empty acting as server with no parent server
    PacketManager* packetManager = PacketManager::Initialize(ENetworkHandleType::Client, settings);

    packetManager->RegisterPacketComponent<TestComponent>({}, &TestComponentFunction);
    
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