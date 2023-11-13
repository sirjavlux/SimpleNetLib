#pragma once
#include <memory>
#include <vector>
#include <tge/math/vector2.h>
#include <tge/math/vector4.h>
#include <tge/math/color.h>

#ifndef _RETAIL
#define DEBUG_DRAW_LINE(aFrom, aTo, aColor) Tga::DebugDrawer::DrawLine(aFrom, aTo, aColor);
#define DEBUG_DRAW_LINES(aFrom, aTo, aColor, aCount) Tga::DebugDrawer::DrawLines(aFrom, aTo, aColor, aCount);
#define DEBUG_DRAW_ARROW(aFrom, aTo, aColor, aArrowHeadSize) Tga::DebugDrawer::DrawArrow(aFrom, aTo, aColor, aArrowHeadSize);
#define DEBUG_DRAW_CIRCLE(aFrom, aRadius, aColor) Tga::DebugDrawer::DrawCircle(aFrom, aRadius, aColor);
#else
#define DEBUG_DRAW_LINE(aFrom, aTo, aColor)
#define DEBUG_DRAW_LINES(aFrom, aTo, aColor, aCount)
#define DEBUG_DRAW_ARROW(aFrom, aTo, aColor, aArrowHeadSize)
#define DEBUG_DRAW_CIRCLE(aFrom, aRadius, aColor) 
#endif

#ifndef _RETAIL
#define MAXSAMPLES 100
namespace Tga
{
	class Text;
	struct LinePrimitive;
	class PerformanceGraph;
	struct LineMultiPrimitive;
	class Texture;
	class DebugDrawer
	{
		friend class ErrorManager;
	public:
		DebugDrawer(bool aIsEnabled);
		~DebugDrawer(void);
		void Init();
		void Update(float aTimeDelta);
		void Render();
		void DrawLine(Vector2f aFrom, Vector2f aTo, Color aColor = Color(1, 1, 1, 1));
		void DrawArrow(Vector2f aFrom, Vector2f aTo, Color aColor = Color(1, 1, 1, 1), float aArrowHeadSize = 1.0f);
		void DrawCircle(Vector2f aPos, float aRadius, Color aColor = Color(1, 1, 1, 1));

	private:
		void ShowErrorImage();
		double CalcAverageTick(int newtick);

		int myNumberOfRenderedLines = 0;
		static constexpr int myMaxLines = 10000;
		std::unique_ptr<std::array<LinePrimitive, myMaxLines>> myLineBuffer = nullptr;

		std::unique_ptr<Text> myFPS;
		std::unique_ptr<Text> myMemUsage;
		std::unique_ptr<Text> myDrawCallText;
		std::unique_ptr<Text> myCPUText;
		std::unique_ptr<Text> myErrorsText;
		Texture* myErrorTexture;

		int tickindex;
		int ticksum;
		int ticklist[MAXSAMPLES];
		float myShowErrorTimer;

		int myIsEnabled;
		std::unique_ptr<PerformanceGraph> myPerformanceGraph;
		unsigned short myRealFPS;
		unsigned long myRealFPSAvarage;
	};
}
#else

namespace Tga
{
	class DebugDrawer
	{
	public:
		DebugDrawer(int /*aFlagset*/) {}
		~DebugDrawer() {}
		void Init() {}
		void Update(float /*aTimeDelta*/) {}
		void Render() {}
		void DrawLine(Vector2f /*aFrom*/, Vector2f /*aTo*/, Vector4f /*aColor*/) {}
		void DrawLines(Vector2f* /*aFrom*/, Vector2f* /*aTo*/, Color* /*aColor*/, unsigned int /*aCount*/) {}

		void DrawArrow(Vector2f /*aFrom*/, Vector2f /*aTo*/, Vector4f /*aColor*/, float /*aArrowHeadSize*/) {}
		static void DrawArrow(Vector2f /*aFrom*/, Vector2f /*aTo*/, Color aColor = Color(1, 1, 1, 1), float aArrowHeadSize = 1.0f) { aColor; aArrowHeadSize; }
		void DrawCircle(Vector2f /*aPos*/, float /*aRadius*/, Color aColor = Color(1, 1, 1, 1)) { aColor; }
		void ShowErrorImage() {}
	private:

	};
}


#endif