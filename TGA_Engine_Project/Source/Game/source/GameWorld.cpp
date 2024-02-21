#include "stdafx.h"

#include "GameWorld.h"

#include "Entity/EntityManager.h"
#include "Entity/Entities/PlayerShipEntity.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld()
{
	EntityManager::End();
}

void GameWorld::Init()  
{
	EntityManager::Initialize();

	// Register entities
	EntityManager::Get()->RegisterEntityTemplate<PlayerShipEntity>(NetTag("player.ship"));
}

void GameWorld::Update(const float InTimeDelta)
{
	EntityManager::Get()->UpdateEntities(InTimeDelta);
}

void GameWorld::Render()
{
	EntityManager::Get()->RenderEntities();
}