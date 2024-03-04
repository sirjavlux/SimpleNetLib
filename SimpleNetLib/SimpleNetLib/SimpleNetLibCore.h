#pragma once

#include "NetIncludes.h"

namespace Net
{
class NetHandler;

class SimpleNetLibCore
{
public:
    static SimpleNetLibCore* Initialize();
    static SimpleNetLibCore* Get();
    static void End();

    void Update();
    
    SimpleNetLibCore();
    ~SimpleNetLibCore();

    void SetUpServer(PCWSTR InServerAddress = DEFAULT_SERVER_ADDRESS_WIDE, u_short InServerPort = DEFAULT_SERVER_PORT);
    void ConnectToServer(PCWSTR InServerAddress = DEFAULT_SERVER_ADDRESS_WIDE, u_short InServerPort = DEFAULT_SERVER_PORT);

    void DisconnectFromServer();

    NetHandler* GetNetHandler() { return netHandler_; }
    
private:
    static SimpleNetLibCore* instance_;
    NetHandler* netHandler_ = nullptr;
};
}
