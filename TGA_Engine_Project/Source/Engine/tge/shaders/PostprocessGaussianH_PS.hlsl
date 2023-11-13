#include "PostprocessStructs.hlsli"
#include "Common.hlsli"

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;

	float texelSize = 1.0f / Resolution.x;
	float3 blurColor = 0;

	unsigned int kernelSize = 5;
	float start = (((float)(kernelSize)-1.0f) / 2.0f) * -1.0f;
	for (unsigned int idx = 0; idx < kernelSize; idx++)
	{
		float2 uv = input.myUV + float2(texelSize * (start + (float)idx), 0.0f);
		float3 resource = FullscreenTexture1.Sample(DefaultSampler, uv).rgb;
		blurColor += resource * GaussianKernel5[idx];
	}

	result.myColor.rgb = blurColor;
	result.myColor.a = 1.0f;
	
	return result;
}