#pragma once

#include "../Entity/Entities/Entity.hpp"

class BulletEntity : public Entity
{
public:
	void Init() override;
	
	void Update(float InDeltaTime) override;
	void FixedUpdate() override;

	void SetShooter(const uint16_t InIdentifier) { shooterId_ = InIdentifier; }
	uint16_t GetShooterId() const { return shooterId_; }

private:
	uint16_t shooterId_ = 0;
	float speed_ = 5.f;;
};
