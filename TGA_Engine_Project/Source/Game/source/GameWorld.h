#pragma once

class Player;

#define FFR_TIME 1.f / 60.f
#define WORLD_SIZE_X 12.f
#define WORLD_SIZE_Y 12.f
#define WORLD_BG_GENERATION_SIZE_X (WORLD_SIZE_X + 2.f)
#define WORLD_BG_GENERATION_SIZE_Y (WORLD_SIZE_Y + 2.f)

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