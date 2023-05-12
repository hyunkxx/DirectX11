
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
texture2D	g_BackTexture;

float4		g_vCamPosition;
float3		g_vColor;

float2		g_vUV;
float		g_fAlpha;
float		g_fDisPower;

float		g_ihlslWinSizeX = 1280.f;
float		g_ihlslWinSizeY = 720.f;

//
texture2D	g_DissolveTexture;
float		g_fDissolveRate;
float		g_fTimeAcc;
float4		g_vDissolveColor;

texture2D	g_NoiseTexture;



float rand(float seed) {
	return frac(15547.3f * sin(frac(94.3f*seed + 10579.6f) * frac(76.5f*seed + 8437.2f)));
}

float rand(float2 seed) {

	if (seed.y > seed.x)
		return lerp(seed.x, seed.y, (seed.x + 50.) / (seed.y + 250.));
	return lerp(seed.x, seed.y, (seed.y + 50.) / (seed.x + 250.));;
}

float smooth1(float x) {
	return x * x * (3.f - 2.f * x);
}

float smooth2(float x) {
	return x * x * x * (x * (x * 6.f - 15.f) + 10.f);
}


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

struct VS_OUT_DISTORTION
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
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

VS_OUT_DISTORTION VS_MAIN_DISTORTION(VS_IN In)
{
	VS_OUT_DISTORTION Out = (VS_OUT_DISTORTION)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent)).xyz;

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


struct PS_IN_DISTORTION
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vDistortion : SV_TARGET1;
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
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_CLAMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	float2 vUV = clamp(In.vTexUV + g_vUV, 0.f, 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);


	vMtrlDiffuse.a = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));

	if (vMtrlDiffuse.a > 0.1f)
		Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}
PS_OUT	PS_MAIN_ALPHA_CLAMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vUV = clamp(In.vTexUV + g_vUV, 0.f, 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;
	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

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
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_DISTORTION(PS_IN_DISTORTION In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vNormalDesc = g_NoiseTexture.Sample(LinearClampSampler, In.vTexUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	vNormal.y *= -1.f;

	if (0.f == vNormalDesc.a)
		discard;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	float3 vWorldPos = In.vWorldPos.xyz + (normalize(vNormal) - (In.vNormal.xyz)) * g_fDisPower;

	matrix matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float4 vProjPos = mul(float4(vWorldPos, 1.f), matVP);

	float2 vUV;
	vUV.x = saturate((vProjPos.x / vProjPos.w) * 0.5f + 0.5f);
	vUV.y = saturate((vProjPos.y / vProjPos.w) * -0.5f + 0.5f);

	float4 vDepth = g_DepthTexture.Sample(LinearClampSampler, vUV);


	Out.vDiffuse = g_BackTexture.Sample(LinearClampSampler, vUV);


	Out.vDiffuse.a = 1.f;

	return Out;
}

PS_OUT	PS_MAIN_DISTORTION_MASK(PS_IN_DISTORTION In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vNormalDesc = g_NoiseTexture.Sample(LinearClampSampler, In.vTexUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	vNormal.y *= -1.f;

	if (0.f == vNormalDesc.a)
		discard;

	if (0.99f <= vNormal.b)
	{
		if (-0.05f < vNormal.r && 0.05f > vNormal.r)
		{
			if (-0.05f < vNormal.g && 0.05f > vNormal.g)
			{
				discard;
			}
		}
	}

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	float3 vWorldPos = In.vWorldPos.xyz + (normalize(vNormal) * g_fDisPower);

	matrix matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float4 vProjPos = mul(float4(vWorldPos, 1.f), matVP);

	float2 vUV;
	vUV.x = saturate((vProjPos.x / vProjPos.w) * 0.5f + 0.5f);
	vUV.y = saturate((vProjPos.y / vProjPos.w) * -0.5f + 0.5f);

	Out.vDiffuse = g_BackTexture.Sample(LinearClampSampler, vUV);


	Out.vDiffuse.a = 1.f;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
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
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
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
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
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
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
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
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass Default_Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	pass Mask_Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_MASK();
	}

}