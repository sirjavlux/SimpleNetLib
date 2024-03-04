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
    Net::SimpleNetLibCore::Initialize();

    Net::SimpleNetLibCore::Get()->SetUpServer(); // TODO: Custom port if not locally(Good idea to set this up in a file outside project to read from)
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
