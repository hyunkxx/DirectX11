#include "SHADER_DEFINES.hpp"

float4x4  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4  g_ProjMatrixInv, g_ViewMatrixInv;
float4x4  g_PipeProjMatrix, g_PipeViewMatrix;

texture2D g_SourTexture, g_DestTexture;
texture2D g_DepthTexture;

float4 g_vCamPosition;
float g_fFogStart, g_fFogEnd;

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

	Out.vColor = g_SourTexture.Sample(LinearBorderSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_EXTRACTION_BLACK_WHITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vSourceColor = g_SourTexture.Sample(LinearBorderSampler, In.vTexUV);

	if(vSourceColor.r >= 0.3f)
		Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	else
		Out.vColor = float4(0.f, 0.f, 0.f, 1.f);

	return Out;
}

PS_OUT PS_EXTRACTION_COMBINE_ADD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vSourColor = g_SourTexture.Sample(LinearBorderSampler, In.vTexUV);
	vector vDestColor = g_DestTexture.Sample(LinearBorderSampler, In.vTexUV);

	Out.vColor = vSourColor + vDestColor;

	return Out;
}

PS_OUT PS_EXTRACTION_COMBINE_BLEND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vSourColor = g_SourTexture.Sample(LinearBorderSampler, In.vTexUV);
	vector vDestColor = g_DestTexture.Sample(LinearBorderSampler, In.vTexUV);

	Out.vColor = (vSourColor + vDestColor) / 2.f;

	return Out;
}

PS_OUT PS_EXTRACTION_FINAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vSourColor = g_SourTexture.Sample(LinearBorderSampler, In.vTexUV);
	vector vDestColor = g_DestTexture.Sample(LinearBorderSampler, In.vTexUV);

	if (vSourColor.a == 0.f)
		Out.vColor = vDestColor;
	else
		Out.vColor = vSourColor;

	return Out;
}

PS_OUT PS_EXTRACTION_FINAL_FOG(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vFogColor = float4(0.9f, 0.9f, 0.9f, 1.0f);

	vector vSourColor = g_SourTexture.Sample(LinearBorderSampler, In.vTexUV);
	vector vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);

	//float fViewZ = vDepthDesc.y * g_Far;

	//vector vPosition;
	//vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	//vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	//vPosition.z = vDepthDesc.x * fViewZ;
	//vPosition.w = fViewZ;
	//
	//if (vDepthDesc.w == 0.5f)
	//{
	//	float distance = length(vPosition.xyz);
	//	float fogFactor = saturate((distance - 500.f) / 0.1f);

	//	Out.vColor = lerp(vFogColor, vSourColor, fogFactor);

	//}
	//else
	//{
		float fFogFactor = saturate((g_fFogEnd - vDepthDesc.y * g_Far) / (g_fFogEnd - g_fFogStart));
		Out.vColor = fFogFactor * vSourColor + (1.0 - fFogFactor) * vFogColor;
	//}

	return Out;
}   


technique11 DefaultTechnique
{
	pass Extraction_Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION();
	}

	pass Extraction_BlackWhite_Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION_BLACK_WHITE();
	}

	pass Extraction_AddCombine_Pass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION_COMBINE_ADD();
	}

	pass Extraction_BlendCombine_Pass3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION_COMBINE_BLEND();
	}

	pass Extraction_Final_Pass4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION_FINAL();
	}

	pass Extraction_Final_FOG_Pass5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXTRACTION_FINAL_FOG();
	}

}