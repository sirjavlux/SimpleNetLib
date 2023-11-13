#pragma once

namespace Tga
{

struct Vertex
{
	// Debug layout
	// float4 float4 float4 float4 float4 float2 float2 float2 float2 float3 float3 float3
	
	Vector4f Position = {0,0,0,0 };
	Vector4f VertexColors[4]
	{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	};

	Vector2f UVs[4]
	{
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};

	Vector3f Normal = {0, 0, 0};
	Vector3f Tangent = { 0, 0, 0 };
	Vector3f Binormal = { 0, 0, 0 };
	Vector4f Bones = { 0, 0, 0, 0 };
	Vector4f Weights = { 0, 0, 0, 0 };

	Vertex() = default;

	Vertex(float X, float Y, float Z, float R, float G, float B, float A, float U, float V)
	{
		Position = { X, Y, Z, 1 };
		VertexColors[0] = { R, G, B, A };
		UVs[0] = { U, V };
	}

	Vertex(float X, float Y, float Z, float nX, float nY, float nZ, float tX, float tY, float tZ, float bX, float bY, float bZ, float R, float G, float B, float A, float U, float V)
	{
		Position = { X, Y, Z , 1 };
		VertexColors[0] = { R, G, B, A };
		UVs[0] = { U, V };
		Normal = { nX, nY, nZ };
		Tangent = { tX, tY, tZ };
		Binormal = { bX, bY, bZ };
	}
};

} // namespace Tga
