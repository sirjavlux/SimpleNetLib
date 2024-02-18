#include "stdafx.h"

#include "Server.h"
#include "source/GameWorld.h"

int main(const int /*argc*/, const char* /*argc*/[])
{
	Server server;
	server.Init();

	GameWorld gameWorld;
	gameWorld.Init();

	using namespace std::chrono;
	
	steady_clock::time_point lastUpdateTime = steady_clock::now();
	
	while (server.IsRunning())
	{
		const steady_clock::time_point currentTime = steady_clock::now();
		const duration<float> deltaTime = duration_cast<duration<float>>(currentTime - lastUpdateTime);
		
		server.Update();
		
		gameWorld.Update(deltaTime.count());
		
		lastUpdateTime = currentTime;
	}
	
	return 0;
}
