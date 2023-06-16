//***************************************************************************************
// ShadowDebug.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Include common HLSL code.
#include "Common.hlsl"

#define posIndex 7
#define normalIndex 8
#define albedoIndex 9

struct VertexIn
{
	float3 PosL    : POSITION;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

    // Already in homogeneous clip space.
    vout.PosH = float4(vin.PosL, 1.0f);
	
	vout.TexC = vin.TexC;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gTextureMaps[albedoIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	float3 fresnelR0 = {1.0,1.0,1.0};
    float shininess = gTextureMaps[normalIndex].Sample(gsamAnisotropicWrap, pin.TexC).a;
	float3 normal = gTextureMaps[normalIndex].Sample(gsamAnisotropicWrap, pin.TexC).xyz;
	float4 PosW = gTextureMaps[posIndex].Sample(gsamAnisotropicWrap, pin.TexC);

	float3 toEyeW = normalize(gEyePosW - PosW.xyz);

	float4 ambient = gAmbientLight*diffuseAlbedo;
	
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    float4 shadowPosH = mul(PosW, gShadowTransform);
    shadowFactor[0] = CalcShadowFactor(shadowPosH);

	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, PosW.xyz,
		normal, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// Add in specular reflections.
	float3 r = reflect(-toEyeW, normal);
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, normal, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
	
	// Common convention to take alpha from diffuse albedo.
	litColor.a = diffuseAlbedo.a;
    return litColor;
}


