#include "stdafx.h"

#include "Server.h"

int main(const int /*argc*/, const char* /*argc*/[])
{
	Server server;
	
	server.Init();

	while (server.IsRunning()) {
		server.Update();
	}
	
	return 0;
}