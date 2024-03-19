#pragma once

class Server
{
public:
    Server();
    ~Server();

    void Init();
    bool IsRunning() const { return bIsRunning_; }

    void End();

    void Update();
    
private:
    bool bIsRunning_ = true;
    
};