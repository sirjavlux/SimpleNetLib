#include "stdafx.h"
#include "AnimatedObject.h"

#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/DebugDrawer.h>

void AnimatedObject::Init(EntityControlType aType)
{
	myControlType = aType;
	myType = EntityType::AnimatedObject;

	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };
	{
		sharedData.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/mygga.png");

		mySpriteInstance.myPivot = { 0.5f, 0.5f };
		mySpriteInstance.myPosition = GetPosition() * resolution;
		mySpriteInstance.mySize = Tga::Vector2f{ .15f, .15f }*resolution.y;
		mySpriteInstance.myColor = Tga::Color(1, 1, 1, 1);
	}
}

void AnimatedObject::InitServer(EntityControlType aType)
{
	myControlType = aType;
	myType = EntityType::AnimatedObject;
}

void AnimatedObject::Update(float /*aDeltaTime*/)
{

}

void AnimatedObject::Render()
{
	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	mySpriteInstance.myPosition = GetPosition() * resolution;

	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	spriteDrawer.Draw(sharedData, mySpriteInstance);
}