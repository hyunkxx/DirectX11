
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_DepthTexture;

// NoramlMap
texture2D	g_NormalTexture;

float3		g_vEditionColor;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal	: NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;

	float4	vRight : TEXCOORD1;
	float4	vUp : TEXCOORD2;
	float4	vLook : TEXCOORD3;
	float4	vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct VS_OUT_SHADOW
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float4 vShadowDepth : TEXCOORD1;
};

// NoramlMap
struct VS_OUT_NORMALMAP
{
	float4			vPosition : SV_POSITION;
	float4			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;

	float3			vTangent : TANGENT;
	float3			vBiNormal : BINORMAL;
};

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vOutNormal : SV_TARGET3;
};

struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}


VS_OUT_NORMALMAP VS_MAIN_NORMALMAP(VS_IN In)
{
	VS_OUT_NORMALMAP		Out = (VS_OUT_NORMALMAP)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector		vPosition = mul(float4(In.vPosition, 1.0f), TransformMatrix);

	/*vector vNormal = mul(float4(In.vNormal, 0.0f), TransformMatrix);
	vector vTangent = mul(float4(In.vTangent, 0.0f), TransformMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vBiNormal = normalize(cross(Out.vNormal, Out.vTangent));*/

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.0f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent.xyz, 0.0f), g_WorldMatrix)).rgb;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
	Out.vShadowDepth = Out.vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

// NoramlMap
struct PS_IN_NORMALMAP
{
	float4			vPosition : SV_POSITION;
	float4			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;

	float3			vTangent : TANGENT;
	float3			vBiNormal : BINORMAL;
};

PS_OUT PS_MAIN_NORMALMAP(PS_IN_NORMALMAP In)
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

	/* 투영 스페이스 상의 z , 뷰 스페이스 상의 z (vProjPos.w 에 저장되어 있음) -> 뷰 공간서의 최대 z 값 Far -> Far 로 나눠서 0 ~ 1 사이로 뷰 의 z값 보관 */
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}


PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_SHADOW	PS_MAIN_SHADOW(VS_OUT_SHADOW In)
{
	PS_OUT_SHADOW Out = (PS_OUT_SHADOW)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if(vDiffuse.a > 0.1f)
		Out.vShadowDepth = vector(In.vShadowDepth.z / In.vShadowDepth.w, In.vShadowDepth.w / g_Far, 0.4f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_EDITIONCOLOR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vMtrlDiffuse.rgb *= g_vEditionColor;

	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_NORMALMAP_EDITIONCOLOR(PS_IN_NORMALMAP In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector			vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	vMtrlDiffuse.rgb *= g_vEditionColor;

	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	// RS_CullBack RS_Default

	//나뭇잎
	pass Instance_Model_Pass0
	{
		SetRasterizerState(RS_CullBack);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
	pass Instance_Model_NoramlMap_Pass1
	{
		SetRasterizerState(RS_CullBack);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMALMAP();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMALMAP();
	}

	pass Shadow_Pass2
	{
		SetRasterizerState(RS_ShadowDepth);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}
	// 커스텀 컬러 노말맵 x
	pass Instance_Model_EditionColor_Pass3
	{
		SetRasterizerState(RS_CullBack);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		// RS_Default RS_Wireframe
		// DS_Default DS_ZTest_NoZWrite
		// BS_Default BS_AlphaBlend
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EDITIONCOLOR();
	}
	// 커스텀 컬러 노말맵 o
	pass Instance_Model_EditionColor_NoramlMap_Pass4
	{
		SetRasterizerState(RS_CullBack);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMALMAP();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMALMAP_EDITIONCOLOR();
	}
}