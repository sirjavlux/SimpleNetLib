#pragma once
#include "Packet/PacketDefinitions.hpp"

class Client
{
public:
    Client();
    ~Client();

    void Init();
    bool IsRunning() const { return bIsRunning; }

    void End();

    void Update(const float InDeltaTime);
    
private:
    bool bIsOpen_ = true;
    char addressBuffer_[24] = DEFAULT_SERVER_ADDRESS;
    char portBuffer_[6];
    
    bool bIsRunning = true;
};
