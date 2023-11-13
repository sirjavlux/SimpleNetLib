#include "PostprocessStructs.hlsli"

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput returnValue;
	returnValue.myColor.rgb = FullscreenTexture1.Sample(DefaultSampler, input.myUV).rgb;
	returnValue.myColor.a = 1.0f;
	return returnValue;
}