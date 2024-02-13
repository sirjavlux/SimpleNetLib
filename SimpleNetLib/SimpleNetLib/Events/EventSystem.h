#pragma once

#include "../NetIncludes.h"

namespace Net
{
    class EventSystem
    {
    public:
        static EventSystem* Initialize();
        static EventSystem* Get();
        static void End();

        EventSystem();
        ~EventSystem();

        // Connection events
        DynamicMulticastDelegate<const sockaddr_storage&> onClientConnectEvent;
        DynamicMulticastDelegate<const sockaddr_storage&, ENetDisconnectType> onClientDisconnectEvent;
    
    private:
        static EventSystem* instance_;
    };
}