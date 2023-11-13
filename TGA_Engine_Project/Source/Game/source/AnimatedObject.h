#pragma once
#include "Entity.h"

class AnimatedObject : public Entity
{
public:
	void Init(EntityControlType aType = EntityControlType::None) override;
	void InitServer(EntityControlType aType = EntityControlType::None) override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:
	Tga::Sprite2DInstanceData mySpriteInstance = {};
	Tga::SpriteSharedData sharedData = {};

	friend class Server;
};