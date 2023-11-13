#include "common.hlsli"

struct VertexInputType
{
    float4 position : POSITION;
	uint4  vertexIndex : TEXCOORD0;
	float4 instanceTransform0 : TEXCOORD1;
	float4 instanceTransform1 : TEXCOORD2;
	float4 instanceTransform2 : TEXCOORD3;
	float4 instanceTransform3 : TEXCOORD4;
	float4 instanceColor : TEXCOORD5;
	float4 instanceUV : TEXCOORD6;
	float4 uvRect : TEXCOORD7;
};

static uint2 textureRectLookup[6] =
	{
		uint2(0, 1),
		uint2(0, 3),
		uint2(2, 1),
		uint2(2, 3),
		uint2(2, 1),
		uint2(0, 3),
	};
	
float2 GetUVRect(float4 aRect, uint aIndex)
{
	uint2 theLookuped = textureRectLookup[aIndex];
	return float2(aRect[theLookuped.x], aRect[theLookuped.y]);
}

SpriteVertexToPixel main(VertexInputType input)
{
	SpriteVertexToPixel output;
	
	float2 resolution = Resolution.xy;
	float ratio = resolution.y / resolution.x;

	float4x4 transform = float4x4(input.instanceTransform0, input.instanceTransform1, input.instanceTransform2, input.instanceTransform3);

	float4 vertexWorldPos = mul(input.position, transform);
	float4 vertexViewPos = mul(WorldToCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(CameraToProjection, vertexViewPos);

	float3x3 toWorldRotation = (float3x3)transform;
	output.normal = mul(float3(0.f, 0.f, -1.f), toWorldRotation);

	output.position = vertexProjectionPos;
	output.worldPosition = vertexWorldPos;

	float2 textureRect = GetUVRect(input.uvRect, input.vertexIndex.x);
	output.tex = input.instanceUV.xy + (textureRect * input.instanceUV.zw);
	
	output.color = input.instanceColor;	

    return output;
}