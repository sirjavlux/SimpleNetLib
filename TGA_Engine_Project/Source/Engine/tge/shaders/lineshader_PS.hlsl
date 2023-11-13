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

float4 main(PixelInputTypeLine input) : SV_TARGET
{
	float4 textureColor = input.color;
    return textureColor;
}