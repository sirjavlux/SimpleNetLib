#define NUMBER_OF_LIGHTS_ALLOWED 8 
#define MAX_ANIMATION_BONES 64 
#define USE_LIGHTS
#define USE_NOISE

int GetNumMips(TextureCube cubeTex)
{
	int iWidth = 0;
	int iheight = 0;
	int numMips = 0;
	cubeTex.GetDimensions(0, iWidth, iheight, numMips);
	return numMips;
}

cbuffer CommonBuffer : register(b0) 
{
	float4 Resolution; //myResolution.x = screen width, myResolution.y = screen height, myResolution.z = unset, myResolution.w = unset
	float4 Timings; //myTimings.x = totaltime, myTimings.y = delta time, myTimings.z = unset, myTimings.w = unset

	float4x4 WorldToCamera;
	float4x4 CameraToProjection;
	float4 CameraPosition;
	float NearPlane;
	float FarPlane;
	float Unused0;
	float Unused1;

};

cbuffer LightConstantBufferData : register(b1)
{
	struct PointLightData
	{
		float4 Position;
		float4 Color;
		float Range;
		float3 garbage; // Padding, don't use arrays!
	} myPointLights[NUMBER_OF_LIGHTS_ALLOWED];

	uint NumberOfLights;
	int NumEnvMapMipLevels;
	float garbage0;
	float garbage1;

	float4 AmbientLightColorAndIntensity;
	float4 DirectionalLightDirection;
	float4 DirectionalLightColorAndIntensity;
};

cbuffer CustomShapeConstantBufferData : register(b2) 
{
	float4x4 ModelToWorld;
};

struct SpriteVertexToPixel
{
	float4 position		:	SV_POSITION;
	float4 worldPosition	:	POSITION;
	float2 tex : TEXCOORD0;
	float4 color : TEXCOORD2;
	float3 normal			:	NORMAL;
};

struct ModelVertexInput
{
	float4 position	    :	POSITION;
	float4 vertexColor0	:	COLOR0;
	float4 vertexColor1	:	COLOR1;
	float4 vertexColor2	:	COLOR2;
	float4 vertexColor3	:	COLOR3;
	float2 texCoord0	:	TEXCOORD0;
	float2 texCoord1	:	TEXCOORD1;
	float2 texCoord2	:	TEXCOORD2;
	float2 texCoord3	:	TEXCOORD3;
	float3 normal		:	NORMAL;
	float3 tangent		:	TANGENT;
	float3 binormal	    :	BINORMAL;
	float4 boneIndices  :   BONES;
	float4 weights      :   WEIGHTS;
};

struct ModelVertexToPixel
{
	float4 position			:	SV_POSITION;
	float4 worldPosition	:	POSITION;
	float  depth		    :	DEPTH;
	float4 vertexColor0		:	COLOR0;
	float4 vertexColor1		:	COLOR1;
	float4 vertexColor2		:	COLOR2;
	float4 vertexColor3		:	COLOR3;
	float2 texCoord0		:	TEXCOORD0;
	float2 texCoord1		:	TEXCOORD1;
	float2 texCoord2		:	TEXCOORD2;
	float2 texCoord3		:	TEXCOORD3;
	float3 normal			:	NORMAL;
	float3 tangent			:	TANGENT;
	float3 binormal			:	BINORMAL;
};

struct InstancedPixelInputType
{
	float4 position			:	SV_POSITION;
	float4 worldPosition	:	POSITION;
	float2 tex				:	TEXCOORD0;
	float4 color			:	TEXCOORD2;
	float3 normal			:	NORMAL;
	uint instanceId			:	SV_InstanceID;
};

struct PixelOutput
{
	float4 color		:	SV_TARGET;
};

TextureCube environmentTexture : register(t0);

Texture2D albedoTexture		: register(t1);
Texture2D normalTexture		: register(t2);
Texture2D materialTexture	: register(t3);

SamplerState defaultSampler : register(s0);

cbuffer BoneBuffer : register(b3)
{
	float4x4 Bones[MAX_ANIMATION_BONES];
};

cbuffer ObjectBuffer : register(b2)
{
	float4x4 ObjectToWorld;
}


float2x2 ComputeRotation(float aRotation) 
{
	float c = cos(aRotation); 
	float s = sin(aRotation);
	return float2x2(c, -s, s, c);
}


// This gets Log Depth from worldPosition
float GetLogDepth(float4 worldPosition)
{
	float4 cameraPos = mul(WorldToCamera, worldPosition);
	float4 projectionPos = mul(CameraToProjection, cameraPos);
	return projectionPos.z / projectionPos.w;
}

float GetLinDepth(float4 worldPosition)
{
	float logDepth = GetLogDepth(worldPosition);
	return NearPlane / (FarPlane - logDepth * (FarPlane - NearPlane));
}

// Converts Log Depth to Lin Depth
float LogDepthToLinDepth(float depth)
{
	return NearPlane / (FarPlane - depth * (FarPlane - NearPlane));
}

// Get screen texture coordinates from world position?
float2 GetScreenCoords(float4 worldPosition)
{
	float4 worldToView = mul(WorldToCamera, worldPosition);
	float4 viewToProj = mul(CameraToProjection, worldToView);

	float2 projectedTextureCoords;
	projectedTextureCoords.x = viewToProj.x / viewToProj.w / 2.0f + 0.5f;
	projectedTextureCoords.y = viewToProj.y / viewToProj.w / 2.0f + 0.5f;

	return projectedTextureCoords;
}