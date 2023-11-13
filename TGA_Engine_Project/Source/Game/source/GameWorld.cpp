#include "stdafx.h"

#include "GameWorld.h"

GameWorld::GameWorld()
{}

GameWorld::~GameWorld() 
{}

void GameWorld::Init()  
{

}

void GameWorld::Update(float /*aTimeDelta*/)
{

}

void GameWorld::Render()
{
	for (std::pair<short, Entity*> entity : myEnteties)
		entity.second->Render();
}