﻿#pragma once

#include <chrono>

#include "../NetIncludes.h"

enum class ENetworkHandleType;
enum class EPacketHandlingType : uint8_t;

enum class ENetDisconnectType : uint8_t
{
    TimeOut             = 0,
    ConnectionClosed    = 1,
    Disconnected        = 2,
};

struct NetSettings
{
    explicit NetSettings(const PCWSTR InParentServerAddress = TEXT(""), const PCWSTR InServerAddress = DEFAULT_SERVER_ADDRESS):
        parentServerAddress(InParentServerAddress),
        serverAddress(InServerAddress)
    {
    }

    PCWSTR parentServerAddress;
    u_short parentServerPort = 0; // Leave at 0 if no server connection

    // Self address if server
    PCWSTR serverAddress;
    u_short serverPort = DEFAULT_SERVER_PORT;
};

struct PacketComponentAssociatedData
{
    EPacketHandlingType handlingType;
};
