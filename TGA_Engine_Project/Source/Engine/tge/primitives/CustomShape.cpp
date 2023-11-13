#include "stdafx.h"
#include <tge/primitives/CustomShape.h>
#include <tge/engine.h>
#include <tge/texture/TextureManager.h>

using namespace Tga;
CustomShape::CustomShape()
{
	myIndex = 0;
	myIsDirty = false;
	myTexture = nullptr;
	myBlendState = BlendState::AlphaBlend;
	myHasTexture = false;
}

CustomShape::~CustomShape()
{
}

void CustomShape::Reset()
{
	myPoints.clear();
}

int CustomShape2D::AddPoint(Vector2f aPoint, Color aColor, Vector2f aUV)
{
	SCustomPoint point;
	point.myPosition = Vector3f(aPoint, 0.f);
	point.myColor = aColor;
	point.myIndex = myIndex;
	point.myUV = aUV;
	myPoints.push_back(point);
	if (myPoints.size()%3 == 0)
	{
		myIndex++;
	}
	
	myIsDirty = true;
	return point.myIndex;
}

int CustomShape3D::AddPoint(Vector3f aPoint, Color aColor, Vector2f aUV)
{
	SCustomPoint point;
	point.myPosition = aPoint;
	point.myColor = aColor;
	point.myIndex = myIndex;
	point.myUV = aUV;
	myPoints.push_back(point);
	if (myPoints.size() % 3 == 0)
	{
		myIndex++;
	}

	myIsDirty = true;
	return point.myIndex;
}

// Do this second
void CustomShape::BuildShape()
{
#ifdef _DEBUG
	if (myPoints.size() %3 != 0)
	{
		INFO_PRINT("%s", "CustomShape::BuildShape(): building shape with non even devided by 3 points, invalid! We need it to build triangles!");
		return;
	}

	if (myHasTexture && myTexture)
	{

		bool noUVsSet = true;
		for (SCustomPoint& point : myPoints)
		{
			if (point.myUV.x != 0 && point.myUV.y != 0)
			{
				noUVsSet = false;
			}
		}
		if (noUVsSet)
		{
			ERROR_PRINT("%s", "Warning! A custom shape that have a texture set have no valid UV coordinates, the shape will most likley not be shown. Set valid UV coordinates,")
		}
	}
#endif

	if (!myTexture)
	{
		myTexture = Engine::GetInstance()->GetTextureManager().GetWhiteSquareTexture();
	}

	myIsDirty = false;
}

void Tga::CustomShape::RemovePoint(int aIndex)
{
	for (size_t i = myPoints.size() - 1; i > 0; i--)
	{
		if (myPoints[i].myIndex == aIndex)
		{
			myPoints.erase(myPoints.begin() + i);
			myIsDirty = true;
		}
	}
}

void Tga::CustomShape::SetTexture(const wchar_t* aPath)
{
	myTexture = Engine::GetInstance()->GetTextureManager().GetTexture(aPath);
	myHasTexture = false;
	if (myTexture && !myTexture->myIsFailedTexture)
	{
		myHasTexture = true;
	}
}

