#include "SHADER_DEFINES.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_FinalTexture;
float4		g_vFadeColor = float4(0.f, 0.f, 0.f, 1.f);


float		g_fTimeAcc;
float2		g_Resolution; // 윈도우 사이즈

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
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

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

PS_OUT PS_FADE_IN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_FinalTexture.Sample(LinearBorderSampler, In.vTexUV);
	Out.vColor.rgb = Out.vColor.rgb * g_fTimeAcc;

	return Out;
}

PS_OUT PS_FADE_OUT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_FinalTexture.Sample(LinearBorderSampler, In.vTexUV);
	Out.vColor.rgb = Out.vColor.rgb * (1.f - g_fTimeAcc);

	return Out;
}

technique11 DefaultTechnique
{
	pass Fade_In_Pass0 // 점점 장면이 드러남
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE_IN();
	}

	pass Fade_Out_Pass0 // 점점 장면이 가려짐
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE_OUT();
	}

}
