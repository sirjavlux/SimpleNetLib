#pragma once
#include "Entity.h"

class Flower : public Entity
{
public:
	void Init() override;
	void InitServer() override;
	void Update(float aDeltaTime) override;
	void Render() override;

private:
	Tga::Sprite2DInstanceData mySpriteInstance = {};
	Tga::SpriteSharedData sharedData = {};

	friend class Server;
};