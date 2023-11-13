#include "common.hlsli"

ModelVertexToPixel main(ModelVertexInput input)
{
	ModelVertexToPixel output;
	
	float2 resolution = Resolution.xy;
	float ratio = resolution.y / resolution.x;
		
	float4 pos = input.position;
	float4 skinnedPos = 0;
	uint iBone = 0;
	float fWeight = 0;
	

	// Bone 0
	iBone = input.boneIndices.x;
	fWeight = input.weights.x;
	skinnedPos += fWeight * mul(Bones[iBone], pos);

	// Bone 1
	iBone = input.boneIndices.y;
	fWeight = input.weights.y;
	skinnedPos += fWeight * mul(Bones[iBone], pos);
	
	// Bone 2
	iBone = input.boneIndices.z;
	fWeight = input.weights.z;
	skinnedPos += fWeight * mul(Bones[iBone], pos);

	// Bone 3
	iBone = input.boneIndices.w;
	fWeight = input.weights.w;
	skinnedPos += fWeight * mul(Bones[iBone], pos);
	
	float4 vertexWorldPos = mul(ObjectToWorld, skinnedPos);
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