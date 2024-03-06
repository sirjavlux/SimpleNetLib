#pragma once
#include "Packet/PacketDefinitions.hpp"
#include "source/GameWorld.h"

class Client
{
public:
    Client();
    ~Client();

    void Init();
    bool IsRunning() const { return bIsRunning_; }

    void End();

    void Update(float InDeltaTime);

    void OnCloseConnectionEvent();
    
private:
    GameWorld* gameWorld_ = nullptr;
    
    bool bIsOpen_ = true;
    char addressBuffer_[24] = DEFAULT_SERVER_ADDRESS;
    char portBuffer_[6];
    
    char usernameBuffer_[USERNAME_MAX_LENGTH] = "username";
    
    bool bIsRunning_ = true;
};
