#include "SnakeShape.h"
#include <windows.h>
#include <tge/drawers/CustomShapeDrawer.h>
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>

CSnakeShape::CSnakeShape()
{
	myPosition.Set(0, 0);
	myTime = 0;
	myTime2 = 0;
}

CSnakeShape::~CSnakeShape()
{
}

void CSnakeShape::Init(float aRandom)
{
	myPosition.Set(0, 0);
	myTime = 0;
	myTime2 = 0;
	myRandom = aRandom;
	myTime = myRandom;
	myTime2 = myRandom;
}

#define SNAKE_LENGTH 50
#define SNAKE_SIZE_MULTIPLIER 2.0f
void CSnakeShape::Update(float aTime)
{
	myTime += aTime *2;
	myTime2 += aTime * 3.4f;
	myPosition.x = ((((cosf(myTime) + 1.0f) * 0.3f) + 0.15f) + (sinf(myTime2 * 1.8f) * 0.2f)) * myRandom;
	myPosition.y = ((((sinf(myTime) + 1.0f) * 0.3f) + 0.15f) +(cosf(myTime2 * 1.8f) * 0.3f)) * myRandom;



	myPoints.push_back(myPosition);
	if (myPoints.size() > SNAKE_LENGTH)
	{
		myPoints.erase(myPoints.begin());
	}

	if (myPoints.size() < 2)
	{
		// Need data!
		return;
	}

	myShape.Reset();


	Tga::Vector2f prevNormFirst;
	bool isFirst = true;
	for (int i=0; i< myPoints.size()-1; i+=1)
	{
		if (isFirst)
		{
			Tga::Vector2f lastPos = myPoints[i];
			Tga::Vector2f nextPos = myPoints[i + 1];

			myShape.AddPoint(nextPos, Tga::Color(1, 0, 0, 1));
			myShape.AddPoint(lastPos, Tga::Color(1, 0, 0, 1));

			Tga::Vector2f theNormal = (nextPos - lastPos).Normal() * SNAKE_SIZE_MULTIPLIER;
			myShape.AddPoint(theNormal + nextPos, Tga::Color(0, 1, 0, 1));

			myShape.AddPoint(theNormal + nextPos, Tga::Color(0, 1, 0, 1));
			myShape.AddPoint(lastPos, Tga::Color(1, 0, 0, 1));
			myShape.AddPoint(theNormal + lastPos, Tga::Color(0, 1, 0, 1));

			prevNormFirst = theNormal + nextPos;
		}
		else
		{
			Tga::Vector2f lastPos = myPoints[i];
			Tga::Vector2f nextPos = myPoints[i+1];

			myShape.AddPoint(nextPos, Tga::Color(1, 0, 0, 1));
			myShape.AddPoint(lastPos, Tga::Color(1, 0, 0, 1));

			Tga::Vector2f theNormal = (nextPos - lastPos).Normal() * SNAKE_SIZE_MULTIPLIER;
			myShape.AddPoint(prevNormFirst, Tga::Color(0, 1, 0, 1));

			myShape.AddPoint(theNormal + nextPos, Tga::Color(0, 1, 0, 1));
			myShape.AddPoint(nextPos, Tga::Color(1, 0, 0, 1));
			myShape.AddPoint(prevNormFirst, Tga::Color(0, 1, 0, 1));

			prevNormFirst = theNormal + nextPos;
		}

		isFirst = false;
	}

	myShape.BuildShape();

	Tga::Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer().Draw(myShape);

}
