
#include "Shader_Defines.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_LightPos;
float4 g_LightDir;

float4x4 g_BoneMatrix[256];

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal	: NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;
	
	uint4 vBlendIndices : BLENDINDEX;
	float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vDepth : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct VS_OUT_SHADOW
{
	float4 vPosition : SV_POSITION;
	float4 vShadowDepth : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	matWV, matWVP, matLightWV, matLightWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	AnimMatrix =
		g_BoneMatrix[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrix[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrix[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrix[In.vBlendIndices.w] * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	vector vNormal = mul(float4(In.vNormal, 0.f), AnimMatrix);

	Out.vPosition = mul(vPosition, matWVP);

	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent)).xyz;

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	AnimMatrix =
		g_BoneMatrix[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrix[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrix[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrix[In.vBlendIndices.w] * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	
	Out.vPosition = mul(vPosition, g_WorldMatrix);

	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

	Out.vShadowDepth = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vDepth : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct PS_IN_SHADOW
{
	float4 vPosition : SV_POSITION;
	float4 vShadowDepth : TEXCOORD0;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
};

struct PS_OUT_OUTLINE
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
	float4 vOutNormal : SV_TARGET3;
};

struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
	float4 vLightPos : SV_TARGET1;
};

PS_OUT_OUTLINE	PS_MAIN(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	if (0.1f >= vMtrlDiffuse.a)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	if (Out.vDepth.g * 1000 < 10.f)
		Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth = vector(In.vShadowDepth.z / In.vShadowDepth.w, In.vShadowDepth.w / g_Far, 0.f, 1.f);
	return Out;
}

technique11 DefaultTechnique
{
	pass Model
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

}