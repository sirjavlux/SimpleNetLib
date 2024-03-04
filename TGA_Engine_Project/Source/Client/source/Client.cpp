#include "Client.h"

#include "tge/Engine.h"

#include "SimpleNetLib.h"
#include "source/Entity/RenderManager.h"

Client::Client()
{
    
}

Client::~Client()
{
    
}

void Client::Init()
{
    RenderManager::Initialize();
    
    Net::SimpleNetLibCore::Initialize();

    Net::SimpleNetLibCore::Get()->ConnectToServer(); // TODO: Connect with specified ip, port and join data
}

void Client::End()
{
    Tga::Engine::Shutdown();
    
    Net::SimpleNetLibCore::End();
    bIsRunning = false;
}

void Client::Update(const float InDeltaTime)
{
    Net::PacketManager::Get()->Update();

    bool bIsOpen = true;
    if (ImGui::Begin("test", &bIsOpen))
    {
        
    }
}
