#pragma once

class Server
{
public:
    Server();
    ~Server();

    void Init();
    bool IsRunning() const { return bIsRunning; }

    void End();

    void Update();
    
private:
    bool bIsRunning = true;
    
};