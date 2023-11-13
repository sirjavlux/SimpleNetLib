#include "common.hlsli"


// No idea why this is a thing? We have the world pos, so why read it?
//GetDepth(worldPositionTexture.Sample(defaultSampler, GetScreenCoords(input.myWorldPosition)).xyzw);

float3x3 invertMatrix(float3x3 m)
{
	// computes the inverse of a matrix m
	float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	float invdet = 1 / det;

	float3x3 minv; // inverse of matrix m
	minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
	minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
	minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
	minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
	minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
	minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
	minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
	minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
	minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

	return minv;
}

ModelVertexToPixel main(ModelVertexInput input)
{
	ModelVertexToPixel result;

	float4 pos = input.position;
	float4x4 skinnedMatrix = 0;
	uint iBone = 0;
	float fWeight = 0;

	// Bone 0
	iBone = input.boneIndices.x;
	fWeight = input.weights.x;
	skinnedMatrix += fWeight * Bones[iBone];

	// Bone 1
	iBone = input.boneIndices.y;
	fWeight = input.weights.y;
	skinnedMatrix += fWeight * Bones[iBone];

	// Bone 2
	iBone = input.boneIndices.z;
	fWeight = input.weights.z;
	skinnedMatrix += fWeight * Bones[iBone];

	// Bone 3
	iBone = input.boneIndices.w;
	fWeight = input.weights.w;
	skinnedMatrix += fWeight * Bones[iBone];

	float4 vertexWorldPos = mul(ObjectToWorld, mul(skinnedMatrix, pos));
	float4 vertexViewPos = mul(WorldToCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(CameraToProjection, vertexViewPos);

	float3x3 skinnedRotation = (float3x3)skinnedMatrix;
	float3x3 toWorldRotation = (float3x3)ObjectToWorld;
	//float3 vertexWorldNormal = mul(toWorldRotation, input.myNormal);

	float3 vertexWorldBinormal = mul(toWorldRotation, mul(skinnedRotation, input.binormal));
	float3 vertexWorldTangent = mul(toWorldRotation, mul(skinnedRotation, input.tangent));
	float3 vertexWorldNormal = mul(transpose(invertMatrix(toWorldRotation)), mul(transpose(invertMatrix(skinnedRotation)), input.normal));

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