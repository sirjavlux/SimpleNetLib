#pragma once

#include <tge/render/RenderObject.h>


namespace Tga
{
	class Engine;
	class GraphicsEngine;
	struct LinePrimitive : public RenderObjectSharedData
	{
		Vector4f color;
		Vector3f fromPosition;
		Vector3f toPosition;
	};

	struct LineMultiPrimitive : public RenderObjectSharedData
	{
		Color *colors;
		Vector3f *fromPositions;
		Vector3f *toPositions;
		unsigned int count;
	};
}