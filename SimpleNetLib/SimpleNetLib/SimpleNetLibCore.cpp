#include "SimpleNetLibCore.h"

#include "Events/EventSystem.h"
#include "Packet/PacketManager.h"

#include "Network/NetHandler.h"

namespace Net
{
SimpleNetLibCore* SimpleNetLibCore::instance_ = nullptr;

SimpleNetLibCore* SimpleNetLibCore::Initialize()
{
    if (instance_ == nullptr)
    {
        instance_ = new SimpleNetLibCore();
        instance_->netHandler_ = new NetHandler();
        
        EventSystem::Initialize();
        PacketManager::Initialize();
    }
    
    return instance_;
}

SimpleNetLibCore* SimpleNetLibCore::Get()
{
    return instance_;
}

void SimpleNetLibCore::End()
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

SimpleNetLibCore::SimpleNetLibCore()
{
    
}

SimpleNetLibCore::~SimpleNetLibCore()
{
    PacketManager::End();
    EventSystem::End();
}

void SimpleNetLibCore::SetUpServer(const PCWSTR InServerAddress, const u_short InServerPort)
{
    netHandler_->SetUpServer(InServerAddress, InServerPort);
}

void SimpleNetLibCore::ConnectToServer(const PCWSTR InServerAddress, const u_short InServerPort)
{
    netHandler_->ConnectToServer(InServerAddress, InServerPort);
}

void SimpleNetLibCore::DisconnectFromServer()
{
    netHandler_->DisconnectFromServer();
}
}
