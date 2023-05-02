
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;

float4		g_vCamPosition;
float3		g_vColor;

float2		g_vUV;
float		g_fAlpha;

//
texture2D	g_DissolveTexture;
float		g_fDissolveRate;
float		g_fTimeAcc;
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
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
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

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;
	
	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	vMtrlDiffuse.a = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;
	
	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_CLAMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	float2 vUV = clamp(In.vTexUV + g_vUV , 0.f , 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);


	vMtrlDiffuse.a = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.rgb =	vMtrlDiffuse.rgb * g_vColor.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}
PS_OUT	PS_MAIN_ALPHA_CLAMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	float2 vUV = clamp(In.vTexUV + g_vUV, 0.f, 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vUV = clamp(In.vTexUV + g_vUV, 0.f, 1.f);

	vector	vColor = g_DiffuseTexture.Sample(LinearSampler, vUV);

	if (0.1f > vColor.a)
	{
		Out.vDiffuse = vColor;
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
		return Out;
	}

	vector clrBG = float4(0.f, 0.f, 0.f, 0.f);

	float fTime = (sin(g_fTimeAcc / 2.f));

	float fEdge_width_start = 0.1f;
	float fEdge_width_end = 0.05f;
	float fEdge_width = lerp(fEdge_width_start, fEdge_width_end, smoothstep(0.f, 1.f, fTime));

	float fMyAlpha = lerp(0.f - fEdge_width, 1.f, fTime);

	float2 vUv_mask = vUV;
	float4 vAlphaTex = g_DissolveTexture.Sample(LinearSampler, vUv_mask * 0.3f);

	float fAlpha = step(vAlphaTex.r, fMyAlpha);

	float fEdge = smoothstep(vAlphaTex.r - fEdge_width, vAlphaTex.r, fMyAlpha);

	//float4 vEdgeColor = float4(0.f, 0.1f, 0.79f, 1.f);

	float4 vEdgeColor = float4(g_vColor.rgb, 1.f);

	vEdgeColor *= fEdge * 10.f;

	vColor += vEdgeColor;

	Out.vDiffuse = lerp(vColor, clrBG, fAlpha);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_Clamp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CLAMP();
	}

	pass Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	pass Alpha_Clamp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_CLAMP();
	}

	pass Dissolve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

}