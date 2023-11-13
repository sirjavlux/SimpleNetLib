#include "Common.hlsli"

// No idea why this is a thing? We have the world pos, so why read it?
//GetDepth(worldPositionTexture.Sample(defaultSampler, GetScreenCoords(input.myWorldPosition)).xyzw);

ModelVertexToPixel main(ModelVertexInput input)
{
	ModelVertexToPixel result;

	float4 vertexObjectPos = input.position;
	float4 vertexWorldPos = mul(ObjectToWorld, vertexObjectPos);
	float4 vertexViewPos = mul(WorldToCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(CameraToProjection, vertexViewPos);

	float3x3 toWorldRotation = (float3x3)ObjectToWorld;
	float3 vertexWorldNormal = mul(toWorldRotation, input.normal);
	float3 vertexWorldBinormal = mul(toWorldRotation, input.binormal);
	float3 vertexWorldTangent = mul(toWorldRotation, input.tangent);
	
	result.position = vertexProjectionPos;
	result.worldPosition = vertexWorldPos;
	result.vertexColor0 = input.vertexColor0;
	result.vertexColor1 = input.vertexColor1;
	result.vertexColor2 = input.vertexColor2;
	result.vertexColor3 = input.vertexColor3;

	result.texCoord0 = input.texCoord0;
	result.texCoord1 = input.texCoord1;
	result.texCoord2 = input.texCoord2;
	result.texCoord3 = input.texCoord3;

	result.normal = vertexWorldNormal;
	result.binormal = vertexWorldBinormal;
	result.tangent = vertexWorldTangent;
	
	result.depth = GetLinDepth(vertexWorldPos);
	//result.myDepth = GetLogDepth(vertexWorldPos);
	//result.myDepth = LogDepthToLinDepth(vertexWorldPos.z / vertexWorldPos.w);

	//float4x4 camViewProj = FB_ToCamera * FB_ToProjection;
	//float4 temp = mul(vertexWorldPos, camViewProj);
	//result.myDepth = LogDepthToLinDepth(((temp.w - FB_NearPlane) / FB_FarPlane - FB_NearPlane) * -1);

	//result.myVxColor2 = mul(camViewProj, vertexWorldPos);
	
	return result;
}