#include "common.hlsli"

struct VertexInputType
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
	float4x4 world	:	WORLD;
};

ModelVertexToPixel main(VertexInputType input)
{
	ModelVertexToPixel output;
	
	float2 resolution = Resolution.xy;
	float ratio = resolution.y / resolution.x;
		
	float4 vertexWorldPos = mul(input.world, input.position);
	float4 vertexViewPos = mul(WorldToCamera, vertexWorldPos);

	output.worldPosition = vertexWorldPos;
	output.position = mul(CameraToProjection, vertexViewPos);

	output.vertexColor0 = input.vertexColor0;
	output.vertexColor1 = input.vertexColor1;
	output.vertexColor2 = input.vertexColor2;
	output.vertexColor3 = input.vertexColor3;

	output.texCoord0 = input.texCoord0;
	output.texCoord1 = input.texCoord1;
	output.texCoord2 = input.texCoord2;
	output.texCoord3 = input.texCoord3;

    return output;  
}