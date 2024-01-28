#pragma once

#include "../NetIncludes.h"

class EventSystem
{
public:
    static EventSystem* Initialize();
    static EventSystem* Get();
    static void End();

    EventSystem();
    ~EventSystem();

    // Connection events
    DynamicMulticastDelegate<const NetTarget&> onClientConnectEvent;
    DynamicMulticastDelegate<const NetTarget&, ENetDisconnectType> onClientDisconnectEvent;
    
private:
    static EventSystem* instance_;
};