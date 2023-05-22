#include "SHADER_DEFINES.hpp"

#define COUNT 16
#define FARCLIP 10.0
#define BIAS 0.3

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_DiffuseTexture, g_NormalTexture, g_DepthTexture;
texture2D g_NoiseTexture;

float g_fRadius = 0.00003f;

float3 avKernel[8] =
{
	normalize(float3(1, 1, 1)) * 0.125f,
	normalize(float3(-1,-1,-1)) * 0.250f,
	normalize(float3(-1,-1, 1)) * 0.375f,
	normalize(float3(-1, 1,-1)) * 0.500f,
	normalize(float3(-1, 1 ,1)) * 0.625f,
	normalize(float3(1,-1,-1)) * 0.750f,
	normalize(float3(1,-1, 1)) * 0.875f,
	normalize(float3(1, 1,-1)) * 1.000f
};

//노이즈 생성
float3 g_vRandom[16] =
{
	float3(0.2024537f,  0.8412054f,  -0.9060141f),
	float3(-0.2200423f, 0.6282339f,  -0.8275437f),
	float3(0.3677659f,  0.1086345f,  -0.4466777f),
	float3(0.8775856f,  0.4617546f,  -0.6427765f),

	float3(0.7867433f,  -0.1414792f, -0.1567597f),
	float3(0.4839234f,  -0.8252324f, -0.1453405f),
	float3(0.3598349f,  -0.4292492f, -0.3300123f),
	float3(0.7867433f,  -0.8021398f,  0.0731243f),

	float3(-0.7357433f, -0.5521398f,  0.2341853f),
	float3(-0.4527333f, -0.2536398f,  0.0635934f),
	float3(-0.4129283f, -0.2594984f,  0.5934882f),
	float3(-0.8439491f,  0.2021398f, -0.3623629f),

	float3(-0.8212491f,  0.1468278f,  0.2205823f),
	float3(-0.4053491f, -0.8234398f,  0.4683472f),
	float3(-0.6634293f,  0.2021398f,  0.6349358f),
	float3(-0.0001596f,  0.2385584f,  0.8343872f),
};

//랜덤한 노말 생성
float3 RandomNormal(float2 TexUV)
{
	float noiseX = (frac(sin(dot(TexUV, float2(15.8989f, 76.132f) * 1.f)) * 46336.23745f));
	float noiseY = (frac(sin(dot(TexUV, float2(11.9899f, 62.223f) * 2.f)) * 34748.34744f));
	float noiseZ = (frac(sin(dot(TexUV, float2(13.3238f, 63.122f) * 3.f)) * 59998.47362f));

	return normalize(float3(noiseX, noiseY, noiseZ));
};

struct tagSSAO_In
{
	float2 vUV;
	float fDepth;
	float fViewZ;
	float3 vNormal;
};

struct tagSSAO_Out
{
	float4 vAmbient;
};

tagSSAO_Out GetSSAO(tagSSAO_In In)
{
	tagSSAO_Out Out = (tagSSAO_Out) 0.f;

	float3 vRay;
	float3 vReflect;
	float2 vRandomUV;
	float fOccNormal;

	int iColor = 0;

	for (int i = 0; i < 16; ++i)
	{
		vRay = reflect(RandomNormal(In.vUV), g_vRandom[i]);
		vReflect = normalize(reflect(vRay, In.vNormal)) * g_fRadius;
		vReflect.x *= -1.f;

		vRandomUV = In.vUV + vReflect.xy;

		vector NearDepthInfo = g_DepthTexture.Sample(LinearSampler, vRandomUV);
		fOccNormal = NearDepthInfo.g * g_Far;

		if (fOccNormal < In.fDepth + 0.01f)
			++iColor;
	}

	Out.vAmbient = abs((iColor / 16.f) - 1.f);
	return Out;
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

PS_OUT PS_SSAO(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDepth = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vNormal = g_NormalTexture.Sample(LinearClampSampler, In.vTexUV);

	if (vDepth.b == 0.5f)
	{
		Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
		return Out;
	}

	tagSSAO_In SSAO_In = (tagSSAO_In)0;
	vNormal = float4(normalize(vNormal * 2.f - 1.f).xyz, vNormal.a);
	SSAO_In.vUV = In.vTexUV;
	SSAO_In.vNormal = vNormal.rgb;
	SSAO_In.fViewZ = 0; // vDepth.x * g_Far; no use
	SSAO_In.fDepth = vDepth.g * g_Far;

	tagSSAO_Out SSAO_Out = GetSSAO(SSAO_In);

	Out.vColor = 1.f - SSAO_Out.vAmbient;
	return Out;
}

//// SSAO2
PS_OUT PS_SSAO_2(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vSSAOParams = float4(1.f, 1.f, 1.f, 1.f);
	vector vDepth = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vNormal = g_NormalTexture.Sample(LinearClampSampler, In.vTexUV);

	if (vDepth.b == 0.5f)
	{
		Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
		return Out;
	}

	float3 random = RandomNormal(In.vTexUV);
	random = random * 2.0f - 1.0f;

	float fRadius = g_fRadius;
	float fPixelDepth = g_DepthTexture.Sample(LinearBorderSampler, In.vTexUV).g;
	float fDepth = fPixelDepth * g_Far;
	float3 vKernelScale = float3(fRadius / fDepth, fRadius / fDepth, fRadius / g_Far);

	float fOcclusion = 0.0f;
	for (int j = 1; j < 3; j++)
	{
		float3 random = RandomNormal(In.vTexUV.xy * (7.0f + (float)j)).xyz;
		random = random * 2.0f - 1.0f;

		for (int i = 0; i < 8; i++)
		{
			float3 vRotatedKernel = reflect(avKernel[i], random) * vKernelScale;
			float fSampleDepth = g_DepthTexture.Sample(LinearBorderSampler, vRotatedKernel.xy + In.vTexUV.xy).r;
			float fDelta = max(fSampleDepth - fPixelDepth + vRotatedKernel.z, 0);
			float fRange = abs(fDelta) / (vKernelScale.z * vSSAOParams.z);

			fOcclusion += lerp(fDelta * vSSAOParams.w, vSSAOParams.x, saturate(fRange));
		}
	}

	Out.vColor = fOcclusion / (2.0f * 8.0f);
	Out.vColor = 1.f - lerp(0.1f, 0.6, saturate(Out.vColor.x));

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
		PixelShader = compile ps_5_0 PS_SSAO();
	}

	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SSAO_2();
	}
}