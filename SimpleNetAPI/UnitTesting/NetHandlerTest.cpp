#include "../SimpleNetLib/Packet/Packet.h"
#include "../SimpleNetLib/Packet/PacketComponent.h"
#include "../SimpleNetLib/Packet/PacketManager.h"
#include "gtest/gtest.h"

TEST(NetHandlerTest, InitializeServer)
{
    const NetSettings defaultSettings;
    
    PacketManager::Initialize(ENetworkHandleType::Server, defaultSettings);
}

TEST(NetHandlerTest, InitializeClient)
{
    const NetSettings defaultSettings(DEFAULT_SERVER_ADDRESS);
    
    PacketManager::Initialize(ENetworkHandleType::Client, defaultSettings);
}