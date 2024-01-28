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
    const NetSettings netSettings; // Use default settings for now with local address and default port as 42000
    SimpleNetCore::Initialize(ENetworkHandleType::Server, netSettings);
}

void Server::End()
{
    SimpleNetCore::End();
    bIsRunning = false;
}

void Server::Update()
{
    
}
