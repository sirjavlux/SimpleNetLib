#include "common.hlsli"

ModelVertexToPixel main(ModelVertexInput input)
{
	ModelVertexToPixel output;
	
	float2 resolution = Resolution.xy;
	float ratio = resolution.y / resolution.x;
		
	float4 vertexWorldPos = mul(ObjectToWorld, input.position);
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