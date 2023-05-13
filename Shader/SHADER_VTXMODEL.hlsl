
#include "Shader_Defines.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
float4		g_vCamPosition;
float4		g_vColor;

float4		g_LightPos;

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
	float4 vGlow : SV_TARGET4;
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
		Out.vGlow = float4(vMtrlDiffuse.xyz, 1.f);

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
	float4 vGlow : SV_TARGET4;
};

PS_OUT_SKY PS_MAIN_SKY(PS_IN_SKY In)
{
	PS_OUT_SKY	Out = (PS_OUT_SKY)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	//¾Æ¿ô¶óÀÎ ¾ø´Â µðÆúÆ® ±×¸®±â
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

	//±×¸²ÀÚ ¸Ê
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

	//¾Æ¿ô¶óÀÎ ÀÖ´Â ±×¸®±â
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

	// 3. NormalMap ÀÌ ¾ø´Â ¸Ê °´Ã¼ 
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

	// 4. NormalMap ÀÌ ÀÖ´Â ¸Ê °´Ã¼ 
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

	// 6 ·Îºñ¸Ê ¾îµÓ°Ô ±×¸®±â
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
}