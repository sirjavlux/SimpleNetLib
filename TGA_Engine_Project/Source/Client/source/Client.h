#pragma once

class Client
{
public:
    Client();
    ~Client();

    void Init();
    bool IsRunning() const { return bIsRunning; }

    void End();

    void Update(const float InDeltaTime);
    
private:
    bool bIsRunning = true;
};