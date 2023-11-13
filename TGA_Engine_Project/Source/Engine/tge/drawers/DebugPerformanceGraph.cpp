#include "stdafx.h"

#include <tge/drawers/CustomShapeDrawer.h>
#include <tge/drawers/DebugPerformancegraph.h>
#include <tge/drawers/DebugDrawer.h>
#include <tge/drawers/LineDrawer.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/primitives/LinePrimitive.h>
#include <tge/engine.h>
#include <tge/sprite/sprite.h>
#include <tge/primitives/CustomShape.h>
#include <tge/text/Text.h>

#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>

#include <stdio.h>
#include <psapi.h>


using namespace Tga;

PerformanceGraph::PerformanceGraph(DebugDrawer* aDrawer)
	:myDrawer(aDrawer)
	, myBackground(nullptr)
{
}

PerformanceGraph::~PerformanceGraph(void)
{}

void PerformanceGraph::Init(Tga::Color& aBackgroundColor, Tga::Color& aLineColor, const std::string& aText)
{
	myBackground = std::make_unique<CustomShape2D>();
	myLineColor = aLineColor;

	myBackground->AddPoint(Vector2f(0.f, 0.f), aBackgroundColor);
	myBackground->AddPoint(Vector2f(1.f, 1.f), aBackgroundColor);
	myBackground->AddPoint(Vector2f(1.f, 0.f), aBackgroundColor);

	myBackground->AddPoint(Vector2f(0.f, 0.f), aBackgroundColor);
	myBackground->AddPoint(Vector2f(0.f, 1.f), aBackgroundColor);
	myBackground->AddPoint(Vector2f(1.f, 1.f), aBackgroundColor);
	myBackground->BuildShape();

	myText = std::make_unique<Tga::Text>(L"Text/arial.ttf");
	myText->SetText("--");
	myText->SetColor({ 1, 1, 1, 1.0f });
	myText->SetText(aText);
}


void PerformanceGraph::FeedValue(int aValue)
{
	if (myBuffer.size() > DEBUG_PERFGRAPH_SAMPLES)
	{
		myBuffer.erase(myBuffer.begin());
	}
	myBuffer.push_back(aValue);
}


void PerformanceGraph::Render()
{
	if (!Engine::GetInstance()->IsDebugFeatureOn(DebugFeature::FpsGraph))
	{
		return;
	}

	Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
	Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	float startX = 0.6f * resolution.x;

	int maxVal = 0;
	int minVal = INT_MAX;

	for (unsigned int i = 0; i < myBuffer.size(); i++)
	{
		if (myBuffer[i] > maxVal)
		{
			maxVal = myBuffer[i];
		}
		if (myBuffer[i] < minVal)
		{
			minVal = myBuffer[i];
		}
	}

	Vector2f backgroundPosition = Vector2f(0.6f, 0.90f)*resolution;
	Vector2f backgroundSize = Vector2f(0.4f, 0.11f) * resolution;

	myBackground->SetPosition(backgroundPosition);
	myBackground->SetSize(backgroundSize);

	Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer().Draw(*myBackground);

	float increaseX = 0.4f / (float)DEBUG_PERFGRAPH_SAMPLES * resolution.x;

	

	std::vector<Vector3f> theTos;
	std::vector<Vector3f> theFrom;
	std::vector<Tga::Color> theColors;
	theTos.reserve(myBuffer.size());
	theFrom.reserve(myBuffer.size());
	theColors.reserve(myBuffer.size());

	if (myBuffer.size() > 2)
	{
		float lastY = (float)myBuffer[0] / (float)maxVal;
		lastY = (9.f + lastY) * resolution.y / 10.0f;

		for (unsigned int i = 1; i < myBuffer.size(); i++)
		{
			float thisY = (float)myBuffer[i] / (float)maxVal;

			thisY = (9.f + thisY) * resolution.y / 10.0f;

			float incrWithI = increaseX * ((float)i + 1);

			theTos.push_back(Vector3f(startX + incrWithI, lastY, 0.f));
			theFrom.push_back(Vector3f((startX + incrWithI) + increaseX, thisY, 0.f));
			theColors.push_back(myLineColor);

			lastY = thisY;
		}
	}

	if (theTos.size() > 0)
	{	
		LineMultiPrimitive lines;
		lines.fromPositions = &theFrom[0];
		lines.toPositions = &theTos[0];
		lines.colors = &theColors[0];
		lines.count = static_cast<int>(theTos.size());
		Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer().Draw(lines);
		//myDrawer->DrawLines(&theTos[0], &theFrom[0], &theColors[0], static_cast<int>(theTos.size()));
	}
	
	myText->SetPosition(Vector2f(0.6f, 1.f-0.09f)* resolution);
	myText->SetScale(1.0f);
	myText->Render();

}
