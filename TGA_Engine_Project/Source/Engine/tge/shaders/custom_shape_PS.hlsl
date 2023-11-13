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

SamplerState SampleType;

float4 main(PixelInputTypeCustom  input) : SV_TARGET
{
	float4 Diffuse = input.color;
	input.color = Diffuse;
    return input.color;
}