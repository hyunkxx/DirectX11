
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_DepthTexture;

// NoramlMap
texture2D	g_NormalTexture;

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

VS_OUT_NORMALMAP VS_MAIN_NORMALMAP(VS_IN In)
{
	VS_OUT_NORMALMAP		Out = (VS_OUT_NORMALMAP)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector vPosition = mul(float4(In.vPosition, 1.0f), TransformMatrix);

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

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.0f), g_WorldMatrix));
	Out.vBiNormal = normalize(cross(Out.vNormal, Out.vTangent));

	return Out;
}

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
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 1.f);

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
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.f, 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Instance_Model
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

	pass Instance_Model_NoramlMap
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMALMAP();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMALMAP();
	}
}