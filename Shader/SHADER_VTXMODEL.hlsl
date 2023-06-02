
#include "Shader_Defines.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
float4		g_vCamPosition;
float3		g_vColor;

float4		g_LightPos;

texture2D	g_EmissiveTexture;
texture2D	g_SpecularTexture;
texture2D	g_SSAOTexture;
texture2D   g_MaskTexture;

float		g_fTimeAcc;

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
	float4 vDepth : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct VS_OUT_SHADOW
{
	float4 vPosition : SV_POSITION;
	float4 vShadowDepth : TEXCOORD0;
};

//Dessolve
texture2D g_DissolveTexture;
float  g_fDissolveAmount = 1.f;
float3 g_vDessolveColor = { 1.f, 0.7f, 0.4f }; // 디폴트 색상

float g_fGlowRange = 0.05f;
float g_fGlowFalloff = 0.05f;

float3 g_vMinPoint = { -1.f, -1.f, -1.f };
float3 g_vMaxPoint = { 1.f, 1.f, 1.f };

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
	Out.vDepth = Out.vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent)).xyz;

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;

	matrix	matWV, matWVP;

	Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

	Out.vShadowDepth = Out.vPosition;

	return Out;
}

struct VS_OUT_SKY
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT_SKY VS_MAIN_SKY(VS_IN In)
{
	VS_OUT_SKY		Out = (VS_OUT_SKY)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_Dessolve_XLinear(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	// 로컬 포지션을 그대로 전달함
	Out.vDepth = float4(In.vPosition, 1.f);

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
	float4 vDepth : TEXCOORD2;

	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct PS_IN_SHADOW
{
	float4 vPosition : SV_POSITION;
	float4 vShadowDepth : TEXCOORD0;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
};

struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
};

struct PS_OUT_OUTLINE
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vOutNormal : SV_TARGET3;
	float4 vSpecGlow : SV_TARGET4;
	float4 vGlow : SV_TARGET5;
	float4 vShaderInfo : SV_TARGET6;
	
};

PS_OUT_OUTLINE	PS_MAIN(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	//Out.vOutNormal = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE	PS_Outline(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	
	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	if (vNormalDesc.g > vNormalDesc.b)
		Out.vSpecGlow = float4(vMtrlDiffuse.xyz, 1.f);

	return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(VS_OUT_SHADOW In)
{
	PS_OUT_SHADOW Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth = vector(In.vShadowDepth.z / In.vShadowDepth.w, In.vShadowDepth.w / g_Far, 0.5f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_MAPOBJECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_MAPOBJECT_NORMALMAP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	return Out;
}

PS_OUT_OUTLINE PS_MAIN_MAPOBJECT_NORMALMAP_DARK(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse * 0.5f;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	return Out;
}

struct PS_IN_SKY
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};
struct PS_OUT_SKY
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
	float4 vOutNormal : SV_TARGET3;
	float4 vSpecGlow : SV_TARGET4;
	float4 vGlow : SV_TARGET5;
	float4 vShaderInfo : SV_TARGET6;

};

PS_OUT_SKY PS_MAIN_SKY(PS_IN_SKY In)
{
	PS_OUT_SKY	Out = (PS_OUT_SKY)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vNormal	= float4(0.f, 0.f, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 1.f);
	Out.vDepth = float4(0.f, 0.f, 0.f, 1.f);
	Out.vShaderInfo = float4(0.1f, 0.f, 0.f, 0.f);

	return Out;
}

//모델 글로우
PS_OUT_OUTLINE PS_MAIN_MODEL_SPECGLOW(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	if (vNormalDesc.g > vNormalDesc.b)
		Out.vSpecGlow = float4(vMtrlDiffuse.xyz, 1.f);

	if (vNormalDesc.g > vNormalDesc.b)
		Out.vGlow = vector(0.f, 0.f, 0.f, 0.f);

	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}


PS_OUT_OUTLINE PS_MAIN_MODEL_EMISSIVE(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector			vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	vector			vSpec = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector			vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV);

	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	if (vNormalDesc.g > vNormalDesc.b)
		Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);

	float2 uv = { 0.f, 0.f };
	uv.x = g_fTimeAcc;
	vector vTwinklMask = g_MaskTexture.Sample(LinearClampSampler, uv);
	
	vEmissive.rgb = vEmissive.rgb * g_vColor;
	Out.vGlow = vEmissive * vTwinklMask;
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_Dessolve(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormal = In.vNormal;

	float dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;
	dissolve = dissolve * 0.999f;
	float isVisible = dissolve - g_fDissolveAmount;
	clip(isVisible);

	float isGlowing = smoothstep(g_fGlowRange + g_fGlowFalloff, g_fGlowRange, isVisible);
	float3 vDessolveColor = isGlowing * g_vDessolveColor;

	Out.vDiffuse = vMtrlDiffuse;
	vMtrlDiffuse.a = 1.f;

	Out.vNormal = float4(vNormal.xyz, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vOutNormal = vector(vNormal.xyz, 1.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse += float4(vDessolveColor.xyz, 1.f);
	Out.vGlow = float4(vDessolveColor.xyz, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_Dessolve_XLinear(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormal = In.vNormal;

	float fLocalX = g_vMaxPoint.x - In.vDepth.x;
	float fMaxX = g_vMaxPoint.x - g_vMinPoint.x;

	float isVisible = fLocalX / fMaxX - g_fDissolveAmount;
	clip(isVisible);

	float isGlowing = smoothstep(g_fGlowRange + g_fGlowFalloff, g_fGlowRange, isVisible);
	float3 vDessolveColor = isGlowing * g_vDessolveColor;

	Out.vDiffuse = vMtrlDiffuse;
	vMtrlDiffuse.a = 1.f;

	Out.vNormal = float4(vNormal.xyz, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vOutNormal = vector(vNormal.xyz, 1.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);

	Out.vDiffuse += float4(vDessolveColor.xyz, 1.f);
	Out.vGlow = float4(vDessolveColor.xyz, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	//아웃라인 없는 디폴트 그리기
	pass Model_Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//그림자 맵
	pass Shadow_Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

	//아웃라인 있는 그리기
	pass Outline_Pass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Outline();
	}

	// 3. NormalMap 이 없는 맵 객체 
	pass Model_MapObject_Pass3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAPOBJECT();
	}

	// 4. NormalMap 이 있는 맵 객체 
	pass Model_MapObject_NormalMap_Pass4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAPOBJECT_NORMALMAP();
	}

	// 5. Sky
	pass Model_Sky
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SKY();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SKY();
	}

	// 6 로비맵 어둡게 그리기
	pass Model_MapObject_NormalMap_Dark_Pass6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAPOBJECT_NORMALMAP_DARK();
	}

	// 7 모델 스펙글로우
	pass Model_Glow_Pass7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MODEL_SPECGLOW();
	}

	// 7 모델 이미시브(스펙큘러) : Emissive, SSAO 텍스쳐 필요 >> Chest전용
	pass Model_Emissive_Pass8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MODEL_EMISSIVE();
	}

	// Static Model 전용 디졸브
	pass Dessolve_SModel_Pass9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Dessolve();
	}

	// Weapon용 Z 선형 디졸브
	pass Dessolve_SModel_Pass10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_Dessolve_XLinear();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Dessolve_XLinear();
	}

}