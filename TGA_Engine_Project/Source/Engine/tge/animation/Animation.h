#pragma once
#include <vector>
#include <string>
#include <tge/math/Transform.h>

namespace Tga
{
	struct Animation
	{
		struct Frame
		{
			std::vector<Transform> LocalTransforms;
		};

		std::vector<Frame> Frames;

		// The animation length in frames.
		unsigned int Length;

		// The number of framer per second
		float FramesPerSecond;

		// The animation duration in seconds.
		float Duration;

		std::wstring Name;
	};
}

