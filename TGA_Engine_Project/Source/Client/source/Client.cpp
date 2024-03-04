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

    const std::string portString = std::to_string(DEFAULT_SERVER_PORT);
    std::memcpy(&portBuffer_[0], portString.c_str(), portString.size() > 5 ? 5 : portString.size());
    portBuffer_[5] = '\0';
}

void Client::End()
{
    Tga::Engine::Shutdown();
    
    Net::SimpleNetLibCore::End();
    bIsRunning = false;
}

void Client::Update(const float InDeltaTime)
{
    Net::SimpleNetLibCore::Get()->Update();

    if (!Net::SimpleNetLibCore::Get()->GetNetHandler()->IsRunning())
    {
        if (ImGui::Begin("Connection Window", &bIsOpen_))
        {
            ImGui::InputText("Server Address", &addressBuffer_[0], 24);
            ImGui::InputText("Server Port", &portBuffer_[0], 6);

            if (ImGui::Button("Connect"))
            {
                std::cout << "Connect To Server\n";
                Net::SimpleNetLibCore::Get()->ConnectToServer(); // TODO: Doesn't use address and port at the moment
            }
            
            ImGui::End();
        }
    }
}
