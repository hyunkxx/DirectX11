
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_BoneMatrix[256];

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DissolveTexture;
float		g_fDissolveRate;
float4		g_vDissolveColor;

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
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	matWV, matWVP;

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

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBiNormal = normalize(cross(Out.vNormal, Out.vTangent));

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
};

struct PS_OUTLINE
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
	float4 vOutNormal	: SV_TARGET3;
};


PS_OUTLINE	PS_MAIN(PS_IN In)
{
	PS_OUTLINE Out = (PS_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	if (0.1f >= vMtrlDiffuse.a)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	if(Out.vDepth.g * 1000 < 10.f)
		Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_DISSOLVING(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f >= vMtrlDiffuse.a)
		discard;

	vector vMtrlDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

	if (g_fDissolveRate > vMtrlDissolve.r)
		discard;

	if (vMtrlDissolve.r - g_fDissolveRate < 0.1f)
		vMtrlDiffuse = vMtrlDiffuse + g_vDissolveColor * ((vMtrlDissolve.r - g_fDissolveRate) / 0.1f) * 2.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

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

	pass Model_Dissolving
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVING();
	}
}