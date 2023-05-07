#include "Shader_Defines.hpp"

matrix				g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D			g_DiffuseTexture;

texture2D			g_DiffuseTexture_1;
texture2D			g_DiffuseTexture_2;
texture2D			g_DiffuseTexture_3;
texture2D			g_DiffuseTexture_4;

texture2D			g_FilterTexture;

texture2D			g_BrushTexture;
vector				g_vBrushPos = vector(10.f, 0.f, 0.f, 1.f);
float				g_fBrushRange = 5.f;

struct VS_IN
{
	float3			vPosition : POSITION;
	float3			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
};


struct VS_OUT_PHONG
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT_PHONG VS_MAIN_PHONG(VS_IN In)
{
	VS_OUT_PHONG		Out = (VS_OUT_PHONG)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN_PHONG
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4			vDiffuse : SV_TARGET0;
	float4			vNormal : SV_TARGET1;
	float4			vDepth : SV_TARGET2;
	float4			vOutNormal : SV_TARGET3;
};

/*
PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vSourDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse = vSourDiffuse;

	vector		vBrushColor = vector(0.f, 0.f, 0.f, 0.f);

	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (g_fBrushRange * 2.f),
			((g_vBrushPos.z - g_fBrushRange) - In.vWorldPos.z) / (g_fBrushRange * 2.f));
		vBrushColor = g_BrushTexture.Sample(LinearSampler, vUV);
	}

	Out.vDiffuse = vMtrlDiffuse + vBrushColor;
	Out.vDiffuse.a = 1.f;

	// In.vNormal.xyz => -1 ~ 1 
	// Out.vNormal.xyz => 0 ~ 1 
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	Out.vOutNormal = float4(0.5f, 0.5f, 0.5f, 1.f);

	return Out;
}
*/

PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse_1 = g_DiffuseTexture_1.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDiffuse_2 = g_DiffuseTexture_2.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDiffuse_3 = g_DiffuseTexture_3.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDiffuse_4 = g_DiffuseTexture_4.Sample(LinearSampler, In.vTexUV * 100.f);

	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse = { 0.0f, 0.0f, 0.0f, 0.0f };

	float		fTotalFilter = vFilter.r + vFilter.g + vFilter.b + vFilter.a;

	if (0.0f < fTotalFilter)
	{
		vFilter = vFilter / fTotalFilter;

		vMtrlDiffuse = (vDiffuse_1 * vFilter.r)
			+ (vDiffuse_2 * vFilter.g)
			+ (vDiffuse_3 * vFilter.b)
			+ (vDiffuse_4 * (1.0f - (vFilter.r + vFilter.g + vFilter.b)));
	}
	else
		vMtrlDiffuse = vDiffuse_4;

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a = 1.f;

	// In.vNormal.xyz => -1 ~ 1
	// Out.vNormal.xyz => 0 ~ 1
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	Out.vOutNormal = float4(1.f, 1.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_FLOOR(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 70.f);

	Out.vDiffuse.gb = Out.vDiffuse.r;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.0f);

	return Out;
}

technique11 DefaultTechnique
{
	/* 픽셀단위에서 빛연산을 수행한다. */
	pass Terrain_Phong
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PHONG();
	}

	pass Terrain_Floor
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOOR();
	}
}