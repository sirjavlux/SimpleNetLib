#pragma once
#include <cstdint>

namespace Net
{
enum class EPacketComponent : uint16_t
{
    None                    = 0,

    // Client to server
    ServerConnect           = 1,
    ServerDisconnect        = 2,
    
    // Server to client
    KickedFromServer        = 1001,

    // Other
    ReturnAck               = 2001,
    ServerPing              = 2002
};
}

enum class EPacketHandlingType : uint8_t
{
    None		= 0,
    Ack			= 1,
    
    Size        = 3
};

enum class EAddComponentResult : uint8_t
{
    Success			    = 0,
    InvalidComponent	= 1,
    SizeOutOfBounds	    = 2,
};