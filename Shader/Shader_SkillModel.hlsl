
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
texture2D	g_BackTexture;

float4		g_vCamPosition;
float3		g_vColor;
float3		g_vMaskColor;

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

float3 Set_Mask_Color(float3 vColor)
{
	float3 vCol;
	vCol = (vColor * g_vMaskColor);
	vCol.r = max(vCol.r, vCol.g);
	vCol.r = max(vCol.r, vCol.b);
	vCol.gb = vCol.r;

	return vCol * g_vColor;
}

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

struct PS_OUT_STONE
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;

	float4 vOutNormal : SV_TARGET3;
	float4 vSpecGlow : SV_TARGET4;
	float4 vGlow : SV_TARGET5;
	float4 vShaderInfo : SV_TARGET6;

};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	vMtrlDiffuse.a = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.rgb = Set_Mask_Color(float3(vMtrlDiffuse.rgb));

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

	Out.vDiffuse.rgb = Set_Mask_Color(float3(vMtrlDiffuse.rgb));

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

	Out.vDiffuse.rgb = Set_Mask_Color(float3(vMtrlDiffuse.rgb));

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}
PS_OUT	PS_MAIN_ALPHA_CLAMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vUV = clamp(In.vTexUV + g_vUV, 0.f, 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	vMtrlDiffuse.rgb = Set_Mask_Color(float3(vMtrlDiffuse.rgb));

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
	vColor.rgb *= g_vColor;

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

PS_OUT	PS_MAIN_MIX_DISTORTION(PS_IN_DISTORTION In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vUV;
	float2 vDistotionUV;

	vDistotionUV.x = (In.vTexUV.x) + g_ihlslWinSizeX  * sin(g_fTimeAcc / 50000.f);
	vDistotionUV.y = (In.vTexUV.y) + g_ihlslWinSizeY  * sin(g_fTimeAcc / 50000.f);

	vector BurshColor = g_NoiseTexture.Sample(LinearSampler, vDistotionUV);

	vUV.x = In.vTexUV.x + BurshColor.r / 20.f;
	vUV.y = In.vTexUV.y + BurshColor.g / 20.f;

	//vector DisolveColor = g_DissolveTexture.Sample(LinearSampler, vUV * 10.f);
	vector BaseColor = g_DiffuseTexture.Sample(LinearSampler, clamp(vUV.xy + g_vUV.xy, 0.f, 1.f));
	Out.vDiffuse.a = (BaseColor.r + BaseColor.g + BaseColor.b) / 3.f;
	Out.vDiffuse.a = saturate(Out.vDiffuse.a - (1.f - g_fAlpha));

	Out.vDiffuse.rgb = (BaseColor.rgb * g_vColor) * Out.vDiffuse.a;
	
	if ((BaseColor.r + BaseColor.g + BaseColor.b) / 3.f < 0.1f)
	{
		if (g_fAlpha > 0.3f)
		{
			Out.vDiffuse.a = 0.3f;
		}
		else
		{
			Out.vDiffuse.a = g_fAlpha;
		}
	}
	else
	{
		if (Out.vDiffuse.a < 0.3f)
		{
			if (g_fAlpha < 0.3f)
			{
				Out.vDiffuse.a = g_fAlpha;
			}
			else
			{
				Out.vDiffuse.a = 0.3f;
			}
		}
	}

	return Out;
}

PS_OUT_STONE PS_MAIN_NO_COLOR(PS_IN In)
{
	PS_OUT_STONE Out = (PS_OUT_STONE)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	return Out;
}

PS_OUT_STONE PS_MAIN_NO_COLOR_NO_ALPHA(PS_IN_DISTORTION In)
{
	PS_OUT_STONE Out = (PS_OUT_STONE)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	vector			vNormalDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	if (vMtrlDiffuse.a > 0.1f)
		Out.vGlow = vMtrlDiffuse;

	vMtrlDiffuse.a = 1.f;
	Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	return Out;
}
PS_OUT_STONE PS_MAIN_NO_COLOR_NO_ALPHA_NO_GLOW(PS_IN_DISTORTION In)
{
	PS_OUT_STONE Out = (PS_OUT_STONE)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	vector			vNormalDesc = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	vMtrlDiffuse.a = 1.f;
	Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	return Out;
}



PS_OUT	PS_MAIN_MIX_DISTORTION_N0_CLAMP(PS_IN_DISTORTION In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vUV;
	float2 vDistotionUV;

	vDistotionUV.x = (In.vTexUV.x) + g_ihlslWinSizeX  * sin(g_fTimeAcc / 50000.f);
	vDistotionUV.y = (In.vTexUV.y) + g_ihlslWinSizeY  * sin(g_fTimeAcc / 50000.f);

	vector BurshColor = g_NoiseTexture.Sample(LinearSampler, vDistotionUV);

	vUV.x = In.vTexUV.x + BurshColor.r / 20.f;
	vUV.y = In.vTexUV.y + BurshColor.g / 20.f;

	//vector DisolveColor = g_DissolveTexture.Sample(LinearSampler, vUV * 10.f);
	vector BaseColor = g_DiffuseTexture.Sample(LinearSampler, vUV.xy + g_vUV.xy);
	Out.vDiffuse.a = (BaseColor.r + BaseColor.g + BaseColor.b) / 3.f;
	Out.vDiffuse.a = saturate(Out.vDiffuse.a - (1.f - g_fAlpha));

	Out.vDiffuse.rgb = (BaseColor.rgb * g_vColor) * Out.vDiffuse.a;

	return Out;
}

PS_OUT	PS_MAIN_CLAMP_DISCARD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;
	float2 vUV = In.vTexUV + g_vUV;

	if (vUV.x > 1.f || vUV.y > 1.f || vUV.x < 0.f || vUV.y < 0.f)
		discard;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);


	vMtrlDiffuse.a = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.rgb = Set_Mask_Color(float3(vMtrlDiffuse.rgb));

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));

	if (vMtrlDiffuse.a > 0.1f)
		Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

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

	pass Mix_Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MIX_DISTORTION();
	}

	pass NO_COLOR_SET
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NO_COLOR();
	}

	pass Mix_Distortion_No_Clamp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MIX_DISTORTION_N0_CLAMP();
	}

	pass Deafulat_Clamp_Discard
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CLAMP_DISCARD();
	}
	
	pass NO_COLOR_NO_ALPHA_SET
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NO_COLOR_NO_ALPHA();
	}
	pass NO_COLOR_NO_ALPHA_NO_GLOW_SET
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NO_COLOR_NO_ALPHA_NO_GLOW();
	}
}