#pragma once
#include <cstdint>

enum class EPacketComponent : uint16_t
{
    None                    = 0,

    // Client to server
    ServerConnect           = 1,
    ServerDisconnect        = 2,

    // Server to client
    KickedFromServer        = 1001
};