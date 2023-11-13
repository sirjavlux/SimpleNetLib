#include "common.hlsli"

struct VertexInputType
{
    float4 position : POSITION;
	float4 color : TEXCOORD0;
};

struct PixelInputTypeLine
{
    float4 position : SV_POSITION;
	float4 color : TEXCOORD0;
};

PixelInputTypeLine main(VertexInputType input)
{
    PixelInputTypeLine output;

	float4 vertexViewPos = mul(WorldToCamera, float4(input.position.xyz, 1.f));
	float4 vertexProjectionPos = mul(CameraToProjection, vertexViewPos);

	output.position = vertexProjectionPos;
	output.color = input.color;
    
    return output;
}