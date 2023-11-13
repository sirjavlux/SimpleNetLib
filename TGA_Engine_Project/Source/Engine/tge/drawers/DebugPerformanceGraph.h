#pragma once
#include <vector>

#define DEBUG_PERFGRAPH_SAMPLES 500
namespace Tga
{
	class Text;
	class CustomShape2D;
	class DebugDrawer;
	class PerformanceGraph
	{
	public:
		PerformanceGraph(DebugDrawer* aDrawer);
		~PerformanceGraph(void);
		void Init(Tga::Color& aBackgroundColor, Tga::Color& aLineColor, const std::string& aText);
		void Render();

		void FeedValue(int aValue);
	private:
		std::unique_ptr<CustomShape2D> myBackground;
		std::vector<int> myBuffer;
		DebugDrawer* myDrawer;
		Tga::Color myLineColor;
		std::unique_ptr<Text> myText;

	};
}
