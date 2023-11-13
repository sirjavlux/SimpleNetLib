#include "common.hlsli"

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : TEXCOORD0;
	float2 tex : TEXCOORD1;
};

struct PixelInputTypeCustom
{
    float4 position : SV_POSITION;
	float4 color : TEXCOORD0;
	float2 tex : TEXCOORD2;
};

PixelInputTypeCustom main(VertexInputType input)
{
    PixelInputTypeCustom output;
	
	float4 vertexWorldPos = mul(ModelToWorld, float4(input.position.xyz, 1.f));
	float4 vertexViewPos = mul(WorldToCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(CameraToProjection, vertexViewPos);
	
	output.position = vertexProjectionPos;
	output.tex = input.tex;
    
	output.color = input.color;
	
    return output;
}