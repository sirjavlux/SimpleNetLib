#include "Common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(ModelVertexToPixel input)
{
	PixelOutput result;

	float2 scaledUV = input.texCoord0;
	
	float3 toEye = normalize(CameraPosition.xyz - input.worldPosition.xyz);
	float4 albedo = albedoTexture.Sample(defaultSampler, scaledUV).rgba;
	
	if (albedo.a < 0.5)
	{
		discard;
		result.color = float4(0.f, 0.f, 0.f, 0.f);
		return result;
	}

	float3 normal = normalTexture.Sample(defaultSampler, scaledUV).wyz;
	//float ambientOcclusion = 1 - normal.b;
	float ambientOcclusion = normal.b;
	
	normal = 2.0f * normal - 1.0f;
	normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
	normal = normalize(normal);

	float3x3 TBN = float3x3(
		normalize(input.tangent.xyz),
		normalize(-input.binormal.xyz),
		normalize(input.normal.xyz)
		);

	// Can save an instruction here by instead doing
	// normalize(mul(normal, TBN)); It works because
	// TBN is a 3x3 and therefore TBN^T is the same
	// as TBN^-1. However, it is considered good form
	// to do this.
	TBN = transpose(TBN);
	float3 pixelNormal = normalize(mul(TBN, normal));

	// TGA Channel Pack. MRE-S.
	// Metalness, Roughness, Emissive, Emissive Strength (opt).

	float3 material = materialTexture.Sample(defaultSampler, scaledUV).rgb;

	float metalness = material.r;
	float roughness = material.g;
	float emissive = material.b;

	float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
	float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	
	float3 ambiance = EvaluateAmbiance(
		environmentTexture, pixelNormal, input.normal.xyz,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);

	float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal, roughness,
		DirectionalLightColorAndIntensity.xyz, DirectionalLightDirection.xyz, toEye.xyz
	);

	float3 pointLights = 0; // <- The sum of all point lights.
	for(unsigned int p = 0; p < NumberOfLights; p++)
	{
		pointLights += EvaluatePointLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			myPointLights[p].Color.rgb, myPointLights[p].Color.w, myPointLights[p].Range, myPointLights[p].Position.xyz,
			toEye.xyz, input.worldPosition.xyz);
	}
	
	float3 emissiveAlbedo = albedo.rgb * emissive;
	float3 radiance = ambiance + directionalLight + pointLights + emissiveAlbedo;

	result.color.rgb = radiance;
	result.color.a = 1.0f;
	return result;
}

