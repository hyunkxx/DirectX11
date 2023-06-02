#include "SHADER_DEFINES.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_Value = 12.f;

texture2D g_DiffuseTexture;
texture2D g_LUT;

float3 GetLutColor(float3 ColorIn)
{
	float2 LutSize = float2(0.00390625f, 0.0625f);
	float4 LutUV;

	ColorIn = saturate(ColorIn) * g_Value;
	LutUV.w = floor(ColorIn.b);
	LutUV.xy = (ColorIn.rg + 0.5f) * LutSize;
	LutUV.x += LutUV.w * LutSize.y;
	LutUV.z = LutUV.x + LutSize.y;

	vector Lut1 = g_LUT.Sample(LinearSampler, LutUV.xyzz);
	vector Lut2 = g_LUT.Sample(LinearSampler, LutUV.zyzz);

	return lerp(Lut1.xyz, Lut2.xyz, ColorIn.b - LutUV.w);
}

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_LUT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearBorderSampler, In.vTexUV);
	Out.vColor = float4(GetLutColor(Out.vColor.rgb), 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LUT();
	}
}