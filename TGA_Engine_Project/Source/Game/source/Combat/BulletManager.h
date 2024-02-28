#pragma once

#include <cstdint>

#include "../SimpleNetLib/NetIncludes.h"
#include "../SimpleNetLib/Packet/PacketComponent.h"

class BulletManager
{
public:
	void Initialize();
	
	void Update(float InDeltaTime);

	// Server Function
	void TryShootBulletServer(uint16_t InEntityIdentifier);

private:
	void ShootBullet(uint16_t InEntityIdentifier);
	
	std::unordered_map<uint16_t, std::chrono::steady_clock::time_point> lastTimeBulletShotMap_;

	float bulletShootDelay_ = 0.2f;
};
