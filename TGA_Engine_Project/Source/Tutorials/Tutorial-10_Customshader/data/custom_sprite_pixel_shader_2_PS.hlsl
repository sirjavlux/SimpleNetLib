#include "../../../Engine/tge/shaders/common.hlsli"

SamplerState SampleType;

Texture2D shaderTextures[2] : register( t1 );
Texture2D shaderTextureCustom : register( t4 );

#define MOD2 float2(.16632,.17369)
#define MOD3 float3(.16532,.17369,.15787)

//----------------------------------------------------------------------------------------
///  2 out, 2 in...
float2 Hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * MOD3);
    p3 += dot(p3.zxy, p3.yxz+19.19);
    return frac(float2(p3.x * p3.y, p3.z*p3.x));
}

//---------------------------------------------------------------------------------------
float3 Cells(in float2 p, in float time)
{
    float2 f = frac(p);
    p = floor(p);
	float d = 1.0e10;
    float2 id = float2(0,0);
    time *= 1.5;
    
	for (int xo = -1; xo <= 1; xo++)
	{
		for (int yo = -1; yo <= 1; yo++)
		{
            float2 g = float2(xo, yo);
            float2 n = Hash22(p+g);
            n = n*n*(3.0-2.0*n);
            
			float2 tp = g + .5 + sin(time + 6.2831 * n)*1.2 - f;
            float d2 = dot(tp, tp);
			if (d2 < d)
            {
                // 'id' is the colour code for each squiggle
                d = d2;
                id = n;
            }
		}
	}
	return float3(sqrt(d), id);
}

float4 main(SpriteVertexToPixel input) : SV_TARGET
{
	float2 iResolution = float2(1280, 720);
	float2 uv = input.tex.xy;
	float time = Timings.x;
    float3 col = float3(0, 0, 0);
	float amp = 1.0;
    float size = 4.0 * (abs(frac(time*.01-.5)-.5)*50.0+1.0);
    float timeSlide = .05;
	
	for (int i = 0; i < 20; i++)
    {
        float3 res = Cells(uv * size - size * .5, time);
        float c = 1.0 - res.x;
        // Get a colour associated with the returned id...
        float3 wormCol =  clamp(abs(frac((res.y+res.z)* 1.1 + float3(1.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0) -1.0, 0.0, 1.0);
        c = smoothstep(0.6+amp*.25, 1., c);
        col += amp * c * ((wormCol * .1) + float3(.9, .2, .15));
        amp *= .85;
        time -= timeSlide;
    }
	float4 fragColor = float4(min(col, 1.0), 1.0);

	
	float4 Diffuse = shaderTextureCustom.Sample(SampleType, input.tex);
    Diffuse.xyz = fragColor.xyz;

	return Diffuse;
}