#include "Server.h"

#include <fstream>

#include "SimpleNetLib.h"

#include "json.hpp"
#include "source/Entity/RenderManager.h"

using json = nlohmann::json;

Server::Server()
{
    
}

Server::~Server()
{
    
}

void Server::Init()
{
    std::ifstream f("../EngineAssets/config.json");
    json data = json::parse(f);

    const std::string serverAddress = data["ServerAddress"].get<std::string>();
    const u_short port = static_cast<u_short>(data["Port"].get<int>());
    
    Net::SimpleNetLibCore::Initialize();

    RenderManager::Initialize();
    
    Net::SimpleNetLibCore::Get()->SetUpServer(serverAddress.c_str(), port);
}

void Server::End()
{
    Net::SimpleNetLibCore::End();
    bIsRunning = false;
}

void Server::Update()
{
    Net::SimpleNetLibCore::Get()->Update();
}
