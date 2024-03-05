#pragma once
#include "Packet/PacketDefinitions.hpp"

class Client
{
public:
    Client();
    ~Client();

    void Init();
    bool IsRunning() const { return bIsRunning_; }

    void End();

    void Update(const float InDeltaTime);
    
private:
    bool bIsOpen_ = true;
    char addressBuffer_[24] = DEFAULT_SERVER_ADDRESS;
    char portBuffer_[6];
    
    char usernameBuffer_[24] = "username";
    
    bool bIsRunning_ = true;
};
