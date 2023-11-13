#pragma once
#include <unordered_map>
#include "Entity.h"

class Player;

#define FFR_TIME 1.f / 60.f

class GameWorld
{
public:
	GameWorld(); 
	~GameWorld();

	void Init();
	void Update(float aTimeDelta); 
	void Render();

private:
	std::unordered_map<short, Entity*> myEnteties;

	Player* myPlayer;
};