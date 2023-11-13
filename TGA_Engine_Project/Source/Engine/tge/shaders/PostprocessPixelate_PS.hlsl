#include "PostprocessStructs.hlsli"


cbuffer PixelateEffectBuffer : register(b13)
{
	float EB_PixelSize;
	uint2 EB_Resolution;
	float EB_garbage;
}

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput returnValue;

	const float2 uv = input.myUV;
	const float2 pixelSize = 0.1f / (EB_Resolution / EB_PixelSize);
	const float2 fixedUV = uv + pixelSize / 2.0f;
	const float2 pixelUV = floor(fixedUV / pixelSize) * pixelSize;

	returnValue.myColor.rgb = FullscreenTexture1.Sample(DefaultSampler, pixelUV).rgb;
	returnValue.myColor.a = 1.0f;
	return returnValue;
}