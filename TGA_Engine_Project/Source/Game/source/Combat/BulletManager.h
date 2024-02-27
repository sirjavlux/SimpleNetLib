#pragma once
#include <cstdint>

class BulletManager
{
public:
	void Update(float InDeltaTime); // TODO: Needs to keep track of entity shoot timers and such

	// Server Function
	void TryShootBulletServer(uint16_t InEntityIdentifier); // TODO: Implement shooting
	
private:
	
};
