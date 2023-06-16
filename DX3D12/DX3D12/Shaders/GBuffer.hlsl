//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************


// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float4 PosW    : POSITION0;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;
	
	
    return vout;
}

struct PixelOut
{
	float4 position : SV_Target0;
	float4 normal : SV_Target1;
	float4 albedo : SV_Target2;
};

PixelOut PS(VertexOut pin)
{
	PixelOut Pout;
	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
    Pout.albedo = matData.DiffuseAlbedo;
	//float3 fresnelR0 = matData.FresnelR0;
	float roughness = matData.Roughness;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;
    // Dynamically look up the texture in the array.
    Pout.albedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

	// Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);
	
    Pout.normal = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
    Pout.normal.xyz = NormalSampleToWorldSpace(Pout.normal.rgb, pin.NormalW, pin.TangentW);
    Pout.position = pin.PosW;
    Pout.normal.a = (1.0f - roughness) * Pout.normal.a;
	
   
    return Pout;
}


