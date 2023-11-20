#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdio.h>
#include <cstdint>
#include <algorithm>
#include <array>
#include <exception>
#include <malloc.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include <assert.h>
#include <stdexcept>
#include <Windows.h>
#include <iostream>

#include "Packet/PacketDefinitions.h"

#define NET_LIB_EXPORT __declspec(dllexport)