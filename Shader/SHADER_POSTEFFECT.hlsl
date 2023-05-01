#include "SHADER_DEFINES.hpp"

float4x4  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_SourTexture, g_DestTexture;

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

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;

};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_EXTRACTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_SourTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

technique11 DefaultTechnique
{
	pass Extraction_Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION();
	}
}