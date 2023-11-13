#include "stdafx.h"
#ifndef _RETAIL
#include <tge/drawers/DebugDrawer.h>
#include <tge/drawers/DebugPerformancegraph.h>
#include <tge/drawers/LineDrawer.h>
#include <tge/engine.h>
#include <tge/primitives/LinePrimitive.h>
#include <tge/text/Text.h>
#include <tge/engine.h>
#include <tge/graphics/camera.h>
#include <tge/graphics/DX11.h>
#include <tge/graphics/GraphicsEngine.h>
#include <tge/sprite/sprite.h>
#include <tge/texture/texture.h>
#include <tge/light/LightManager.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/SpriteDrawer.h>

#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>

#include <stdio.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

using namespace Tga;

DebugDrawer::DebugDrawer(bool aIsEnabled)
{
	myIsEnabled = aIsEnabled;
	tickindex = 0;
	ticksum = 0;
	myRealFPS = 0;
	myRealFPSAvarage = 0;
}


DebugDrawer::~DebugDrawer(void)
{}

void DebugDrawer::Init()
{
	myLineBuffer = std::make_unique<std::array<LinePrimitive, myMaxLines>>();

	myFPS = std::make_unique<Tga::Text>(L"Text/arial.ttf", FontSize_24);
	myFPS->SetText("--");
	myFPS->SetColor({ 1, 1, 1, 1.0f });

	myMemUsage = std::make_unique<Tga::Text>(L"Text/arial.ttf");
	myMemUsage->SetText("--");
	myMemUsage->SetColor({ 1, 1, 1, 1.0f });

	myDrawCallText = std::make_unique<Tga::Text>(L"Text/arial.ttf");
	myDrawCallText->SetText("--");
	myDrawCallText->SetColor({ 1, 1, 1, 1.0f });

	myCPUText = std::make_unique<Tga::Text>(L"Text/arial.ttf");
	myCPUText->SetText("--");
	myCPUText->SetColor({ 1, 1, 1, 1.0f });

	myErrorsText = std::make_unique<Tga::Text>(L"Text/arial.ttf");
	myErrorsText->SetText("--");
	myErrorsText->SetColor({ 1, 0.7f, 0.7f, 1.0f });

	myErrorTexture = Engine::GetInstance()->GetTextureManager().GetTexture(L"sprites/error.dds");
	myShowErrorTimer = 0.0f;
	myPerformanceGraph = std::make_unique<PerformanceGraph>(this);
	Tga::Color bgColor(0, 1, 0, 0.4f);
	Tga::Color lineColro(1, 1, 1, 1.0f);
	myPerformanceGraph->Init(bgColor, lineColro, "FPS spike detector");
}


ULONGLONG FixCPUTimings(const FILETIME &a, const FILETIME &b)
{
	LARGE_INTEGER la, lb;
	la.LowPart = a.dwLowDateTime;
	la.HighPart = a.dwHighDateTime;
	lb.LowPart = b.dwLowDateTime;
	lb.HighPart = b.dwHighDateTime;

	return la.QuadPart - lb.QuadPart;
}

float GetCPUUsage(FILETIME *prevSysKernel, FILETIME *prevSysUser,
	FILETIME *prevProcKernel, FILETIME *prevProcUser,
	bool firstRun = false)
{
	FILETIME sysIdle, sysKernel, sysUser;
	FILETIME procCreation, procExit, procKernel, procUser;

	if (!GetSystemTimes(&sysIdle, &sysKernel, &sysUser) ||
		!GetProcessTimes(GetCurrentProcess(), &procCreation, &procExit, &procKernel, &procUser))
	{
		// can't get time info so return
		return -1.;
	}

	// check for first call
	if (firstRun)
	{
		// save time info before return
		prevSysKernel->dwLowDateTime = sysKernel.dwLowDateTime;
		prevSysKernel->dwHighDateTime = sysKernel.dwHighDateTime;

		prevSysUser->dwLowDateTime = sysUser.dwLowDateTime;
		prevSysUser->dwHighDateTime = sysUser.dwHighDateTime;

		prevProcKernel->dwLowDateTime = procKernel.dwLowDateTime;
		prevProcKernel->dwHighDateTime = procKernel.dwHighDateTime;

		prevProcUser->dwLowDateTime = procUser.dwLowDateTime;
		prevProcUser->dwHighDateTime = procUser.dwHighDateTime;

		return -1.;
	}

	ULONGLONG sysKernelDiff = FixCPUTimings(sysKernel, *prevSysKernel);
	ULONGLONG sysUserDiff = FixCPUTimings(sysUser, *prevSysUser);

	ULONGLONG procKernelDiff = FixCPUTimings(procKernel, *prevProcKernel);
	ULONGLONG procUserDiff = FixCPUTimings(procUser, *prevProcUser);

	ULONGLONG sysTotal = sysKernelDiff + sysUserDiff;
	ULONGLONG procTotal = procKernelDiff + procUserDiff;

	return (float)((100.0 * procTotal) / sysTotal);
}


const int CONVERSION_VALUE = 1024;
void DebugDrawer::Update(float aTimeDelta)
{
	if (!myIsEnabled)
	{
		return;
	}

	Vector2ui intResolution = Tga::Engine::GetInstance()->GetRenderSize();
	Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };
	const float scale = 1.f;

	myFPS->SetPosition(Vector2f(0, 1.0f-0.035f) * resolution);
	myFPS->SetScale(scale);

	myMemUsage->SetPosition(Vector2f(0.0f, 1.0f - 0.06f) * resolution);
	myMemUsage->SetScale(scale);

	myDrawCallText->SetPosition(Vector2f(0.0f, 1.0f - 0.09f) * resolution);
	myDrawCallText->SetScale(scale);

	myCPUText->SetPosition(Vector2f(0.0f, 1.0f - 0.12f) * resolution);
	myCPUText->SetScale(scale);

	myErrorsText->SetPosition(Vector2f(0.0f, 1.0f - 0.15f) * resolution);
	myErrorsText->SetScale(scale);


	std::string fpsText;
	
	fpsText.append("FPS: ");

	myRealFPS = static_cast<unsigned short>(1.0f / aTimeDelta);

	static float timeInter = 0;
	static int iterations = 0;
	iterations++;
	timeInter += aTimeDelta;
	myRealFPSAvarage += myRealFPS;
	static int avarageFPS = 0;
	if (timeInter >= 0.3f)
	{
		timeInter = 0;
		avarageFPS = myRealFPSAvarage / iterations;
		myRealFPSAvarage = 0;
		iterations = 0;
	}

	myPerformanceGraph->FeedValue(myRealFPS);

	fpsText.append(std::to_string(avarageFPS));
	myFPS->SetText(fpsText);

	PROCESS_MEMORY_COUNTERS memCounter;
	BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
		&memCounter,
		sizeof(memCounter));

	if (!result)
	{
		return;
	}

	SIZE_T memUsed = (memCounter.WorkingSetSize) / 1024;
	SIZE_T memUsedMb = (memCounter.WorkingSetSize) / 1024 / 1024;

	std::string mem = "Sys Mem: ";
	mem.append(std::to_string(memUsed));
	mem.append("Kb (");
	mem.append(std::to_string(memUsedMb));
	mem.append("Mb)");
	myMemUsage->SetText(mem);

	static FILETIME prevSysKernel, prevSysUser;
	static FILETIME prevProcKernel, prevProcUser;
	float usage = 0.0;

	// first call
	static bool firstTime = true;
	usage = GetCPUUsage(&prevSysKernel, &prevSysUser, &prevProcKernel, &prevProcUser, firstTime);
	firstTime = false;

	std::string cpuText;
	cpuText.append("CPU: ");
	cpuText.append(std::to_string(static_cast<int>(usage)));
	cpuText.append("%");
	myCPUText->SetText(cpuText);

	if (myShowErrorTimer > 0.0f)
	{
		if (!myErrorTexture->myIsFailedTexture)
		{
			SpriteSharedData sharedData = {};
			sharedData.myTexture = myErrorTexture;

			Sprite2DInstanceData instanceData = {};
			instanceData.myPosition = Vector2f(0.9f, 0.0f) * resolution;
			instanceData.mySize = myErrorTexture->myImageSize;
			instanceData.myPivot = Vector2f(0.5f, 0.0f);

			float randomShake = (((rand() % 100) / 100.0f) - 0.5f) * 0.06f;
			instanceData.myRotation = randomShake;
			instanceData.myColor = Tga::Color(1, 1, 1, std::min(myShowErrorTimer, 1.0f));

			Engine::GetInstance()->GetGraphicsEngine().GetSpriteDrawer().Draw(sharedData, instanceData);
		}
	}


	myShowErrorTimer -=  aTimeDelta;
	myShowErrorTimer = std::max(myShowErrorTimer, 0.0f);

	unsigned int errors = Engine::GetInstance()->GetErrorManager().GetErrorsReported();
	if (errors > 0)
	{
		std::string errorsString;
		errorsString.append("Errors: ");
		errorsString.append(std::to_string(errors));
		myErrorsText->SetText(errorsString);
	}

	
}


double DebugDrawer::CalcAverageTick(int newtick)
{
	ticksum -= ticklist[tickindex];  /* subtract value falling off */
	ticksum += newtick;              /* add new value */
	ticklist[tickindex] = newtick;   /* save new value so it can be subtracted later */
	if (++tickindex == MAXSAMPLES)    /* inc buffer index */
		tickindex = 0;

	/* return average */
	return((double)ticksum / MAXSAMPLES);
}

void DebugDrawer::Render()
{
	DX11::BackBuffer->SetAsActiveTarget();

	auto oldAmbientLight = Tga::Engine::GetInstance()->GetLightManager().GetAmbientLight();
	auto oldDirectionalLight = Tga::Engine::GetInstance()->GetLightManager().GetDirectionalLight();
	auto oldCamera = Tga::Engine::GetInstance()->GetGraphicsEngine().GetCamera();

	Tga::Engine::GetInstance()->GetLightManager().SetAmbientLight(AmbientLight{ Color(1.f,1.f,1.f), 1.f });
	Tga::Engine::GetInstance()->GetLightManager().SetDirectionalLight(DirectionalLight{ {}, Color(0.f,0.f,0.f), 0.f });
	Tga::Engine::GetInstance()->GetGraphicsEngine().ResetToDefaultCamera();

	LineDrawer& lineDrawer = Tga::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();

	myPerformanceGraph->Render();
	for (int i=0; i< myNumberOfRenderedLines; i++)
	{
		lineDrawer.Draw((*myLineBuffer)[i]);
	}
	myNumberOfRenderedLines = 0;

	if (myIsEnabled)
	{
		if (Engine::GetInstance()->IsDebugFeatureOn(DebugFeature::Fps))
		{
			myFPS->Render();
		}
		if (Engine::GetInstance()->IsDebugFeatureOn(DebugFeature::Mem))
		{
			myMemUsage->Render();
		}

		if (Engine::GetInstance()->IsDebugFeatureOn(DebugFeature::Cpu))
		{
			myCPUText->Render();
		}
	}

	unsigned int errors = Engine::GetInstance()->GetErrorManager().GetErrorsReported();
	if (errors > 0)
	{
		myErrorsText->Render();
	}

	if (myIsEnabled && Engine::GetInstance()->IsDebugFeatureOn(DebugFeature::Drawcalls))
	{
		std::string drawCalls;
		drawCalls.append("DrawCalls: ");
		int objCount = DX11::GetPreviousDrawCallCount();
		drawCalls.append(std::to_string(objCount));
		myDrawCallText->SetText(drawCalls);
		myDrawCallText->SetColor({ 1, 1, 1, 1 });
		/*
		if (objCount > 300)
		{
			myDrawCallText->SetColor({ 1, 1, 0, 1 });
			if (objCount > 1000)
			{
				myDrawCallText->SetColor({ 1, 0, 0, 1 });
			}
		}
		*/

		myDrawCallText->Render();
	} 

	Tga::Engine::GetInstance()->GetLightManager().SetAmbientLight(oldAmbientLight);
	Tga::Engine::GetInstance()->GetLightManager().SetDirectionalLight(oldDirectionalLight);
	Tga::Engine::GetInstance()->GetGraphicsEngine().SetCamera(oldCamera);
}

void Tga::DebugDrawer::DrawLine(Vector2f aFrom, Vector2f aTo, Color aColor)
{
	if (myNumberOfRenderedLines > myMaxLines)
	{
		return;
	}
	(*myLineBuffer)[myNumberOfRenderedLines].fromPosition = Vector3f(aFrom, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].toPosition = Vector3f(aTo, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].color = aColor.AsVec4();
	myNumberOfRenderedLines++;
}

void Tga::DebugDrawer::ShowErrorImage()
{
	myShowErrorTimer = 4.0f;
}

void DebugDrawer::DrawCircle(Vector2f aPos, float aRadius, Color aColor)
{
	if (myNumberOfRenderedLines+32 > myMaxLines)
	{
		return;
	}
	const short circleResolution = 32;

	struct MultiLine
	{
		void Zero()
		{
			ZeroMemory(colors, sizeof(Color) * 256);
			ZeroMemory(fromPositions, sizeof(Vector2f) * 256);
			ZeroMemory(toPositions, sizeof(Vector2f) * 256);
		}
		Color colors[256];
		Vector3f fromPositions[256];
		Vector3f toPositions[256];
	};

	MultiLine computedLineBuffer;
	computedLineBuffer.Zero();

	Vector3f to;
	bool first = false;
	int currentCount = 0;
	for (int i = 0; i < circleResolution+1; i++)
	{
		float angle = 2.0f * 3.14f * static_cast<float>(i) / static_cast<float>(circleResolution);
		
		if (!first)
		{
			first = true;
			to = Vector3f( aRadius*cos(angle) + aPos.x, aRadius*sin(angle) + aPos.y, 0.f);
			continue;
		}

		computedLineBuffer.colors[currentCount] = aColor;
		computedLineBuffer.fromPositions[currentCount] = to;
		computedLineBuffer.toPositions[currentCount] = Vector3f(aRadius*cos(angle) + aPos.x, aRadius*sin(angle) + aPos.y, 0.f);

		to = computedLineBuffer.toPositions[currentCount];
		currentCount++;		
	}

	LineMultiPrimitive multiLine;

	multiLine.colors = computedLineBuffer.colors;
	multiLine.fromPositions = computedLineBuffer.fromPositions;
	multiLine.toPositions = computedLineBuffer.toPositions;
	multiLine.count = currentCount;

	LineDrawer& lineDrawer = Tga::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();

	lineDrawer.Draw(multiLine);
}

void DebugDrawer::DrawArrow(Vector2f aFrom, Vector2f aTo, Color aColor, float aArrowHeadSize)
{
	if (myNumberOfRenderedLines+3 > myMaxLines)
	{
		return;
	}

	Vector2f direction = aTo - aFrom;
	direction = direction.Normalize();

	direction *= 0.04f * aArrowHeadSize;

	Vector2f theNormal = direction.Normal();
	theNormal *= 0.2f * aArrowHeadSize;

	(*myLineBuffer)[myNumberOfRenderedLines].fromPosition = Vector3f(aFrom, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].toPosition = Vector3f(aTo, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].color = aColor.AsVec4();
	myNumberOfRenderedLines++;

	(*myLineBuffer)[myNumberOfRenderedLines].fromPosition = Vector3f(aTo, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].toPosition = Vector3f(aTo - direction + theNormal, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].color = aColor.AsVec4();
	myNumberOfRenderedLines++;

	(*myLineBuffer)[myNumberOfRenderedLines].fromPosition = Vector3f(aTo, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].toPosition = Vector3f(aTo - direction - theNormal, 0.f);
	(*myLineBuffer)[myNumberOfRenderedLines].color = aColor.AsVec4();
	myNumberOfRenderedLines++;

}
#endif _RETAIL