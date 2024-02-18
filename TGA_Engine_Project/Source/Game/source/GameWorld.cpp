#include "stdafx.h"

#include "GameWorld.h"

#include "Entity/EntityManager.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld()
{
	EntityManager::End();
}

void GameWorld::Init()  
{
	EntityManager::Initialize();
}

void GameWorld::Update(const float InTimeDelta)
{
	EntityManager::Get()->UpdateEntities(InTimeDelta);
}

void GameWorld::Render()
{
	EntityManager::Get()->RenderEntities();
}