#include "../../Engine/tge/shaders/common.hlsli"

SamplerState SampleType;

Texture2D shaderTextures[2] : register( t1 );
Texture2D shaderTextureCustom : register( t4 );

#define DIFFUSE_MAP 0


float2 sincos( float x )
{
	return float2(sin(x), cos(x));
}
float2 rotate2d(float2 uv, float phi)
{
	float2 t = sincos(phi); 
	return float2(uv.x*t.y-uv.y*t.x, uv.x*t.x+uv.y*t.y);
}

float4 main(SpriteVertexToPixel input) : SV_TARGET
{
	float4 Diffuse = shaderTextures[0].Sample(SampleType, input.tex);
	float2 uv = input.tex.xy ;
	
    uv = rotate2d(uv, Timings.x * 10);
    
    float3 c00 = float3(180., 231., 251.) / 255.; //#b4e7fb
    float3 c01 = float3(157., 172., 216.) / 255.; //#9dacd8
    float3 c02 = float3(049., 130., 163.) / 255.; //#3182a3
    float3 c03 = float3(119., 126., 198.) / 255.; //#777ebd
    
    float3 cv = lerp(c00, c01, length(uv-float2(0.,1.)));
    float3 ch = lerp(c02, c03, uv.x);
    
    float3 color = lerp(ch, cv, dot(uv, float2(-1.,1.)));
    
    float4 fragColor = float4(color,1.0);
	
	return Diffuse * fragColor;
}