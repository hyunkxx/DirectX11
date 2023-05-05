#include "SHADER_DEFINES.hpp"

matrix	  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D			g_finalTexture;

texture2D			g_BrightTexture;
texture2D			g_BlurTexture;

texture2D			g_GlowTexture;
texture2D			g_GlowOriTexture;

float				g_fWindowSizeX = 640.f;
float				g_fWindowSizeY = 360.f;

float				g_fWeight[13];

float				g_fColorRange = 24.f;

float fWeight[13] = { 0.0561f , 0.1353f , 0.278f ,0.4868f , 0.7261f , 0.9231f,
						1.f ,
						0.9231f , 0.7261f , 0.4868f , 0.278f , 0.1353f , 0.0561f };

float3 jodieReinhardTonemap(float3 c)
{
	float l = dot(c, float3(0.2126f, 0.7152f, 0.0722f));
	float3 tc = c / (c + 1.0);

	return lerp(c / (l + 1.0), tc, tc);
}

float3 bloomTile(float lod, float2 offset, float2 uv)
{
	return g_GlowTexture.Sample(LinearSampler, uv).rgb;
}

float3 getBloom(float2 uv)
{
	float3 blur = float3(0.f, 0.f, 0.f);

	blur = pow(g_GlowTexture.Sample(LinearSampler, uv).rgb, float3(2.2f, 2.2f, 2.2f)) + blur;
	blur = pow(g_GlowTexture.Sample(LinearSampler, uv).rgb, float3(2.2f, 2.2f, 2.2f)) * 1.3f + blur;
	blur = pow(g_GlowTexture.Sample(LinearSampler, uv).rgb, float3(2.2f, 2.2f, 2.2f)) * 1.6f + blur;
	blur = pow(g_GlowTexture.Sample(LinearSampler, uv).rgb, float3(2.2f, 2.2f, 2.2f)) * 1.9f + blur;
	blur = pow(g_GlowTexture.Sample(LinearSampler, uv).rgb, float3(2.2f, 2.2f, 2.2f)) * 2.2f + blur;

	return blur * g_fColorRange;
}


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
	float4			vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.f);
	float fTotal = 0.f;

	float2 vUV = In.vTexUV;
	float2 vUV2 = 0;

	float fTu = 1.f / (g_fWindowSizeX / 2.f);

	for (int i = -6 ; 7 > i; i++)
	{
		vUV2 = vUV + float2(fTu * i, 0);
		vColor += fWeight[i + 6] * g_BlurTexture.Sample(LinearClampSampler, vUV2);
		fTotal += fWeight[i + 6];
	}

	//vUV2 = vUV + float2(fTu * -6, 0);
	//vColor += fWeight0 * g_BlurTexture.Sample(LinearSampler, vUV2);

	Out.vColor = vColor / fTotal;

	return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float fTotal = 0.f;

	float2 vUV = In.vTexUV;
	float2 vUV2 = 0;

	float fTv = 1.f / (g_fWindowSizeY / 2.f);

	for (int i = -6 ; 7 > i; i++)
	{
		vUV2 = vUV + float2(0 , fTv * i);
		vColor += fWeight[i + 6] * g_BlurTexture.Sample(LinearClampSampler, vUV2);
		fTotal += fWeight[i + 6];
	}

	Out.vColor = vColor / fTotal;

	return Out;
}

PS_OUT PS_MAIN_BRIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFragColor = g_finalTexture.Sample(LinearClampSampler, In.vTexUV);


	if (0.8f <= vFragColor.r || 0.8f <= vFragColor.g || 0.8f <= vFragColor.b)
		Out.vColor = float4(vFragColor.rgb, 1.f);

	return Out;
}

PS_OUT PS_MAIN_GLOW(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vfinalColor = g_finalTexture.Sample(LinearBorderSampler, In.vTexUV);

	/*if (vfinalColor.a == 0.f)
	discard;*/
	// g_GlowOriTexture

	float3 color = pow(g_GlowOriTexture.Sample(LinearBorderSampler, In.vTexUV).rgb * g_fColorRange, float3(2.2f, 2.2f, 2.2f));
	color = pow(color, float3(2.2f, 2.2f, 2.2f));
	color += pow(getBloom(In.vTexUV), float3(2.2f, 2.2f, 2.2f));
	color = pow(color, float3(1.f / 2.2f, 1.f / 2.2f, 1.f / 2.2f));

	color = jodieReinhardTonemap(color);

	vfinalColor = pow(abs(vfinalColor), 2.2f);
	vfinalColor.rgb += color.rgb;

	Out.vColor = pow(abs(vfinalColor), 1.f / 2.2f);

	Out.vColor.a = g_finalTexture.Sample(LinearClampSampler, In.vTexUV).a;

	return Out;
}

PS_OUT PS_MAIN_NOGLOW(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_finalTexture.Sample(LinearBorderSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_GLOW_BLACK_WHITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vfinalColor = g_finalTexture.Sample(LinearBorderSampler, In.vTexUV);

	/*if (vfinalColor.a == 0.f)
	discard;*/
	// g_GlowOriTexture
	
	float3 color = pow(g_GlowOriTexture.Sample(LinearBorderSampler, In.vTexUV).rgb * g_fColorRange, float3(2.2f, 2.2f, 2.2f));
	color = pow(color, float3(2.2f, 2.2f, 2.2f));
	color += pow(getBloom(In.vTexUV), float3(2.2f, 2.2f, 2.2f));
	color = pow(color, float3(1.f / 2.2f, 1.f / 2.2f, 1.f / 2.2f));

	color = jodieReinhardTonemap(color);

	vfinalColor = pow(abs(vfinalColor), 2.2f);
	vfinalColor.rgb += color.rgb;

	Out.vColor = pow(abs(vfinalColor), 1.f / 2.2f);

	if (Out.vColor.r > 0.5f)
		Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	else
		Out.vColor = float4(0.f, 0.f, 0.f, 1.f);

	Out.vColor.a = g_finalTexture.Sample(LinearClampSampler, In.vTexUV).a;

	return Out;
}


PS_OUT PS_MAIN_NOGLOW_BLACK_WHITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_finalTexture.Sample(LinearBorderSampler, In.vTexUV);

	return Out;
}

technique11 DefaultTechnique
{
	pass BlurX_Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
	}

	pass BlurY_Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
	}

	pass Bright_Pass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BRIGHT();
	}

	pass Glow_Pass3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GLOW();
	}

	pass Glow_BlackWhite_Pass4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GLOW_BLACK_WHITE();
	}

	pass Glow_Pass5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOGLOW();
	}

	pass Glow_BlackWhite_Pass6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOGLOW_BLACK_WHITE();
	}
}
