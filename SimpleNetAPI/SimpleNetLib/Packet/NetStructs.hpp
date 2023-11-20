#pragma once

#include "../NetIncludes.h"

enum class EPacketHandlingType : uint8_t;

struct NetSettings
{
    explicit NetSettings(const PCWSTR InServerAddress = TEXT("")) : serverAddress(InServerAddress) { }
    
    const PCWSTR serverAddress;
    u_short serverConnectionPort = 0; // Leave at 0 if no server connection
};

struct PacketComponentAssociatedData
{
    EPacketHandlingType handlingType;
};