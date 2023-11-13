#pragma once
#include <tge/primitives/CustomShape.h>

class CSnakeShape
{
public:
	CSnakeShape();
	~CSnakeShape();
	void Init(float aRandom);
	void Update(float aTime);
	Tga::CustomShape2D myShape;
private:
	std::vector<Tga::Vector2f> myPoints;
	Tga::Vector2f myPosition;
	float myTime;
	float myTime2;
	float myRandom;
};

