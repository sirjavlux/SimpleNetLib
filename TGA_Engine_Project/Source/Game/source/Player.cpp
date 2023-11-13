#include "stdafx.h"
#include "Player.h"

#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/DebugDrawer.h>

void Player::Init()
{
	myType = EntityType::Player;

	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };
	{
		sharedData.myTexture = engine.GetTextureManager().GetTexture(L"Sprites/nocco.png");

		mySpriteInstance.myPivot = { 0.5f, 0.5f };
		mySpriteInstance.myPosition = GetPosition() * resolution;
		mySpriteInstance.mySize = Tga::Vector2f{ .05f, .05f }*resolution.y;
		mySpriteInstance.myColor = Tga::Color(1, 1, 1, 1);
	}
}

void Player::InitServer()
{
    myType = EntityType::Player;
}

void Player::Update(float /*aDeltaTime*/)
{

}

void Player::Render()
{
	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	mySpriteInstance.myPosition = GetPosition() * resolution;

	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	spriteDrawer.Draw(sharedData, mySpriteInstance);
}