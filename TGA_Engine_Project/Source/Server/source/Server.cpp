#include "stdafx.h"

#include "Server.h"
#include "SimpleNetLib.h"

Server::Server()
{
    
}

Server::~Server()
{
    
}

void Server::Init()
{
    const Net::NetSettings netSettings; // Use default settings for now with local address and default port as 42000
    Net::SimpleNetLibCore::Initialize(ENetworkHandleType::Server, netSettings);
}

void Server::End()
{
    Net::SimpleNetLibCore::End();
    bIsRunning = false;
}

void Server::Update()
{
    Net::PacketManager::Get()->Update();
}
