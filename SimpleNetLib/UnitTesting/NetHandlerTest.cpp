#include "gtest/gtest.h"

#include "../SimpleNetLib/Packet/Packet.h"
#include "../SimpleNetLib/Packet/PacketComponent.h"
#include "../SimpleNetLib/Packet/PacketManager.h"

int main(int Argc, char** Argv)
{
    testing::InitGoogleTest(&Argc, Argv);

    return RUN_ALL_TESTS();
}

TEST(NetHandlerTestServer, InitializeServer)
{
    const NetSettings defaultSettings;
    
    PacketManager::Initialize(ENetworkHandleType::Server, defaultSettings);
    PacketManager::End();
}

TEST(NetHandlerTestClient, InitializeClient)
{
    const NetSettings defaultSettings(DEFAULT_SERVER_ADDRESS);
    
    PacketManager::Initialize(ENetworkHandleType::Client, defaultSettings);
    PacketManager::End();
}