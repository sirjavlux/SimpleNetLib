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
    const NetSettings netSettings; // Use default settings for now with local address and default port as 42000
    SimpleNetCore::Initialize(ENetworkHandleType::Server, netSettings);
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
