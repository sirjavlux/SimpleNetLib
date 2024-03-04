#pragma once
#include "UI/HUD.h"

class Player;

class GameWorld
{
public:
	GameWorld(); 
	~GameWorld();

	void InitClient();
	void Init();
	void Update(float InTimeDelta); 
	void Render();

private:
	void GenerateStars() const;

	HUD hud_;
};