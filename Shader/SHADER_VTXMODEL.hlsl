
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
float4		g_vCamPosition;
float4		g_vColor;

//
texture2D	g_DissolveTexture;
float		g_fDissolveRate;
float4		g_vDissolveColor;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal	: NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;
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

struct VS_OUT_EFFECT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT_EFFECT Out = (VS_OUT_EFFECT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

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


struct PS_IN_EFFECT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
};


struct PS_OUTLINE
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vOutNormal : SV_TARGET3;
};

struct PS_OUT_EFFECT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a < 0.2f)
		vMtrlDiffuse = vector(0.95f, 0.95f, 0.95f, 1.f);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}

PS_OUTLINE	PS_Outline(PS_IN In)
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

	if (Out.vDepth.g * 1000 < 10.f)
		Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, In.vProjPos.w / 1000.f);

	return Out;
}


PS_OUT_EFFECT	PS_MAIN_SHIELD(PS_IN_EFFECT In)
{
	PS_OUT_EFFECT Out = (PS_OUT_EFFECT)0;

	vector vLook = normalize(In.vWorldPos - g_vCamPosition);

	float fSpecular = saturate(dot(normalize(In.vNormal) * -1.f, vLook) * 1.3f);

	Out.vColor = g_vColor;
	Out.vColor.a = g_vColor.a * (1.f - fSpecular);
	
	float2		vDepthUV;

	vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthUV);

	float		fOldViewZ = vDepthDesc.y * 1000.f;

	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ);

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

	if (vMtrlDissolve.r - g_fDissolveRate < 0.2f)
		vMtrlDiffuse = vMtrlDiffuse + g_vDissolveColor * ((vMtrlDissolve.r - g_fDissolveRate) / 0.2f) * 2.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}

PS_OUT_EFFECT	PS_MAIN_BLADEWAVE(PS_IN_EFFECT In)
{
	PS_OUT_EFFECT Out = (PS_OUT_EFFECT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a < 0.05f)
		discard;

	Out.vColor = vMtrlDiffuse * g_vColor;

	float2		vDepthUV;

	vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthUV);

	float		fOldViewZ = vDepthDesc.y * 300.f;

	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ);

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

	pass Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Outline();
	}

	pass Shield
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHIELD();
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