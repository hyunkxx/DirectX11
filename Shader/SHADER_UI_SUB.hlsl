
#include "Shader_Defines.hpp"

matrix				g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D			g_DiffuseTexture;
texture2D			g_MaskTexture;
texture2D			g_DepthTexture;

texture2D			g_TwinklMask;

float				g_fTimeAcc = 0.f;
float				g_fFillAmount = 1.f;

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
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
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_Default(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_MaskBlend(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vMask	= g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse * vMask;

	return Out;
}

PS_OUT PS_FillX(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor = vDiffuse;

	if (In.vTexUV.x <= g_fFillAmount)
		discard;

	return Out;
}

PS_OUT PS_FillY(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor = vDiffuse;

	if (In.vTexUV.y <= 1.f - g_fFillAmount)
		discard;

	return Out;
}

PS_OUT PS_Twinkl(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);

	float2 uv = { 0.f, 0.f };
	uv.x = g_fTimeAcc;

	vector vTwinklMask = g_TwinklMask.Sample(LinearClampSampler, uv);

	Out.vColor = vDiffuse;
	Out.vColor.a = Out.vColor.a * vTwinklMask.a;

	return Out;
}

PS_OUT PS_Alpha(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);

	Out.vColor = vDiffuse;
	Out.vColor.a = Out.vColor.a * g_fTimeAcc;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default_Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Default();
	}
	
	pass MaskBlend_Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MaskBlend();
	}

	pass FillX_Pass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FillX();
	}

	pass FillY_Pass3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FillY();
	}

	pass Twinkl_Pass4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Twinkl();
	}

	pass Alpha_Pass4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha();
	}
}