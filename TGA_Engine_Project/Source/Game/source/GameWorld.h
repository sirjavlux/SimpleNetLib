#pragma once

class Player;

class GameWorld
{
public:
	GameWorld(); 
	~GameWorld();

	void Init();
	void Update(float InTimeDelta); 
	void Render();

private:
	void GenerateStars() const;
};