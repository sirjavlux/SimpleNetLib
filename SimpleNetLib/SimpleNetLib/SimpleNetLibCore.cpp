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
        instance_->packetComponentRegistry_ = new PacketComponentRegistry();
        instance_->packetComponentHandleDelegator_ = new PacketComponentDelegator();
        instance_->netHandler_ = new NetHandler();

        instance_->netHandler_->Initialize();
        
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

void SimpleNetLibCore::Update()
{
    netHandler_->Update();
    if (netHandler_->IsRunning())
    {
        PacketManager::Get()->Update();
    }
}

SimpleNetLibCore::SimpleNetLibCore()
{
    
}

SimpleNetLibCore::~SimpleNetLibCore()
{
    PacketManager::End();
    EventSystem::End();
    delete netHandler_;
    delete packetComponentRegistry_;
    delete packetComponentHandleDelegator_;
}

void SimpleNetLibCore::SetUpServer(const PCSTR InServerAddress, const u_short InServerPort)
{
    netHandler_->SetUpServer(InServerAddress, InServerPort);
}

void SimpleNetLibCore::ConnectToServer(const VariableDataObject<CONNECTION_DATA_SIZE>& InVariableData, const PCSTR InServerAddress, const u_short InServerPort)
{
    netHandler_->ConnectToServer(InVariableData, InServerAddress, InServerPort);
}

void SimpleNetLibCore::DisconnectFromServer()
{
    netHandler_->DisconnectFromServer();
}

PacketManager* SimpleNetLibCore::GetPacketManager()
{
    return PacketManager::Get();
}
}
