#include "Client.h"

#include <sstream>

#include "tge/Engine.h"

#include "SimpleNetLib.h"
#include "Events/EventSystem.h"
#include "source/Definitions.hpp"
#include "source/Entity/RenderManager.h"
#include "Utility/StringUtility.hpp"

Client::Client()
{
    
}

Client::~Client()
{
    
}

void Client::OnCloseConnectionEvent()
{
    delete gameWorld_;
    gameWorld_ = nullptr;
}

void Client::Init()
{
    Net::SimpleNetLibCore::Initialize();

    RenderManager::Initialize();
    
    const std::string portString = std::to_string(DEFAULT_SERVER_PORT);
    std::memcpy(&portBuffer_[0], portString.c_str(), portString.size() > 5 ? 5 : portString.size());
    portBuffer_[5] = '\0';

    Net::EventSystem::Get()->onCloseConnectionEvent.AddDynamic<Client>(this, &Client::OnCloseConnectionEvent);

    gameWorld_ = new GameWorld();
    gameWorld_->Init();
    gameWorld_->InitClient();

    Net::NetHandler* netHandler = Net::SimpleNetLibCore::Get()->GetNetHandler();
    netHandler->EnablePacketTracking(true);

    ImGuiIO& imguiIo = ImGui::GetIO();
    imguiIo.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Client::End()
{
    Tga::Engine::Shutdown();
    
    Net::SimpleNetLibCore::End();
    bIsRunning_ = false;

    delete gameWorld_;
    gameWorld_ = nullptr;
}

void Client::Update(const float InDeltaTime)
{
    Net::SimpleNetLibCore::Get()->Update();

    if (Net::SimpleNetLibCore::Get()->GetNetHandler()->IsRunning() && gameWorld_ != nullptr)
    {
        gameWorld_->Update(InDeltaTime);
    }

    if (gameWorld_ == nullptr)
    {
        gameWorld_ = new GameWorld();
        gameWorld_->Init();
        gameWorld_->InitClient();
    }
    
    gameWorld_->Render();
    
    if (!Net::SimpleNetLibCore::Get()->GetNetHandler()->IsRunning())
    {
        if (ImGui::Begin("TheWindow", &bIsOpen_, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::InputText("Server Address", &addressBuffer_[0], 24);
            ImGui::InputText("Server Port", &portBuffer_[0], 6);
            ImGui::InputText("Username", &usernameBuffer_[0], USERNAME_MAX_LENGTH);

            if (ImGui::Button("Connect"))
            {
                VariableDataObject<CONNECTION_COMPONENT_DATA_SIZE> data;
                data << usernameBuffer_;
                
                const u_short port = static_cast<u_short>(std::stoi(portBuffer_));
                Net::SimpleNetLibCore::Get()->ConnectToServer(data, std::string(addressBuffer_).c_str(), port);
            }
            
            ImGui::End();
        }
    }

    const Net::NetHandler* netHandler = Net::SimpleNetLibCore::Get()->GetNetHandler();
    const std::unordered_map<uint16_t, int>& packetComponentsReceived = netHandler->GetPacketComponentsReceivedCounter();
    
    // Packet tracking
    if (ImGui::Begin("TheWindow", &bIsOpen_, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("Packet Components received by type");

        ImGui::Indent(2);
        for (const auto& componentsReceived : packetComponentsReceived)
        {
            std::stringstream streamID;
            streamID << "ID: " << componentsReceived.first;
            std::stringstream stream;
            stream << "Count: " << componentsReceived.second;
            ImGui::LabelText(streamID.str().c_str(), stream.str().c_str());
        }
            
        ImGui::End();
    }
}
