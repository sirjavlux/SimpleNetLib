#include "Client.h"

#include "tge/Engine.h"

#include "SimpleNetLib.h"

Client::Client()
{
    
}

Client::~Client()
{
    
}

void Client::Init()
{
    NetSettings netSettings; // Keep everything at default and local for testing
    netSettings.parentServerAddress = DEFAULT_SERVER_ADDRESS;
    netSettings.parentServerPort = DEFAULT_SERVER_PORT;
    
    SimpleNetCore::Initialize(ENetworkHandleType::Client, netSettings);
}

void Client::End()
{
    Tga::Engine::Shutdown();
    
    SimpleNetCore::End();
    bIsRunning = false;
}

void Client::Update(const float InDeltaTime)
{
    
}
