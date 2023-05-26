
#include "Shader_Defines.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_LightPos;
float4 g_LightDir;

vector g_vCamPosition;

float4x4 g_BoneMatrix[256];

// VTF 뼈 행렬을 저장한 텍스쳐
texture2D	g_VertexTexture;

float4		g_vEyeColor = float4(1.f, 1.f, 0.8f, 1.f);
texture2D	g_EyeBurstTexture;
texture2D	g_EyeMaskTexture;

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;

//Rim Light
float4 g_RimColor = float4(1.f, 0.7f, 0.4f, 1.f);
float g_RimPower = 5.f;

//Dessolve

//기존 디폴트 벨류
//float3 g_vDessolveColor = { 1.f, 0.7f, 0.4f };
//float g_fGlowRange = 0.005f;
//float g_fGlowFalloff = 0.1f;

texture2D g_DissolveTexture;
float  g_fDissolveAmount = 1.f;
float3 g_vDessolveColor = { 0.f, 0.5f, 1.f }; // 디폴트 색상

float g_fGlowRange = 0.05f;
float g_fGlowFalloff = 0.05f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal	: NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;
	
	uint4 vBlendIndices : BLENDINDEX;
	float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vDepth : TEXCOORD2;
	float4 vWorldPos : TEXCOORD3;

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

	matrix	matWV, matWVP, matLightWV, matLightWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	AnimMatrix =
		g_BoneMatrix[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrix[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrix[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrix[In.vBlendIndices.w] * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	vector vNormal = mul(float4(In.vNormal, 0.f), AnimMatrix);

	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vPosition = mul(vPosition, matWVP);

	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent)).xyz;

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	AnimMatrix =
		g_BoneMatrix[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrix[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrix[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrix[In.vBlendIndices.w] * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	
	Out.vPosition = mul(vPosition, g_WorldMatrix);

	Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
	Out.vPosition = mul(Out.vPosition, g_ProjMatrix);

	Out.vShadowDepth = Out.vPosition;

	return Out;
}

float4x4 LoadBoneMatrix(uint iBoneID)
{
	uint iBaseID = iBoneID * 4;
	float fBaseU = iBaseID % 64;
	float fBaseV = iBaseID / 64;

	float4 vRight = g_VertexTexture.Load(int3(fBaseU, fBaseV, 0));
	float4 vUp = g_VertexTexture.Load(int3(fBaseU + 1, fBaseV, 0));
	float4 vLook = g_VertexTexture.Load(int3(fBaseU + 2, fBaseV, 0));
	float4 vPos = g_VertexTexture.Load(int3(fBaseU + 3, fBaseV, 0));

	float4x4 matOut = float4x4(vRight, vUp, vLook, vPos);

	//matOut = transpose(matOut);

	return matOut;
}

VS_OUT VS_MAIN_VTF(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	matrix matX = LoadBoneMatrix(In.vBlendIndices.x);
	matrix matY = LoadBoneMatrix(In.vBlendIndices.y);
	matrix matZ = LoadBoneMatrix(In.vBlendIndices.z);
	matrix matW = LoadBoneMatrix(In.vBlendIndices.w);

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	AnimMatrix =
		matX * In.vBlendWeights.x +
		matY * In.vBlendWeights.y +
		matZ * In.vBlendWeights.z +
		matW * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	vector vNormal = mul(float4(In.vNormal, 0.f), AnimMatrix);

	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent)).xyz;

	return Out;
}

VS_OUT_SHADOW VS_MAIN_VTF_SHADOW(VS_IN In)
{
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;

	matrix	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);


	matrix matX = LoadBoneMatrix(In.vBlendIndices.x);
	matrix matY = LoadBoneMatrix(In.vBlendIndices.y);
	matrix matZ = LoadBoneMatrix(In.vBlendIndices.z);
	matrix matW = LoadBoneMatrix(In.vBlendIndices.w);

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	AnimMatrix =
		matX * In.vBlendWeights.x +
		matY * In.vBlendWeights.y +
		matZ * In.vBlendWeights.z +
		matW * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	vector vNormal = mul(float4(In.vNormal, 0.f), AnimMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vShadowDepth = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vDepth : TEXCOORD2;
	float4 vWorldPos : TEXCOORD3;

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
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
};

struct PS_OUT_OUTLINE
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal	: SV_TARGET1;
	float4 vDepth	: SV_TARGET2;
	float4 vOutNormal : SV_TARGET3;
	float4 vSpecGlow : SV_TARGET4;
	float4 vGlow : SV_TARGET5;
	float4 vShaderInfo : SV_TARGET6;

};

struct PS_OUT_SHADOW
{
	float4 vShadowDepth : SV_TARGET0;
};

PS_OUT_OUTLINE	PS_MAIN(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	Out.vGlow = float4(0.f, 0.f, 0.f, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_Outline(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	if (vNormalDesc.g > vNormalDesc.b)
		Out.vSpecGlow = float4(vMtrlDiffuse.xyz, 1.f);

	Out.vGlow = float4(0.f, 0.f, 0.f, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_Eye(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	Out.vNormal	= vector(1.f, 1.f, 1.f, 0.f);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
	Out.vGlow = float4(0.f, 0.f, 0.f, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_Eye_Burst(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vEyeMask = g_EyeMaskTexture.Sample(LinearClampSampler, In.vTexUV);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vMtrlDiffuse.r = vMtrlDiffuse.r + vEyeMask.r;
	vMtrlDiffuse.g = vMtrlDiffuse.g + vEyeMask.r;

	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	vEyeMask.g = 0.f;

	vector vEyeBust = g_EyeBurstTexture.Sample(LinearClampSampler, In.vTexUV) * g_vEyeColor;
	Out.vSpecGlow = vEyeBust + vEyeMask;

	Out.vGlow = float4(0.f, 0.f, 0.f, 1.f);

	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW Out = (PS_OUT_SHADOW)0;

	Out.vShadowDepth = vector(In.vShadowDepth.z / In.vShadowDepth.w, In.vShadowDepth.w / g_Far, 0.5f, 1.f);

	return Out;
}

PS_OUT_OUTLINE PS_RimLight(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vCamDir = normalize(In.vWorldPos - g_vCamPosition);
	float rim = 0;
	rim = 1 - saturate(dot(In.vNormal, -vCamDir));

	rim = pow(rim, g_RimPower);

	float4 rimColor = rim * g_RimColor;
	Out.vDiffuse = rimColor;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vGlow = Out.vDiffuse;

	Out.vShaderInfo = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_DiffuseAddRim(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vCamDir = normalize(In.vWorldPos - g_vCamPosition);
	float rim = 0;
	rim = 1 - saturate(dot(In.vNormal, -vCamDir));

	rim = pow(rim, 1.f);

	float4 rimColor = rim * float4(1.f, 1.f, 1.f, 1.f);
	Out.vDiffuse = saturate(rimColor * 2.5f);

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal	= float4(vNormalDesc.xyz * 2.f - 1.f, 1.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);
	if ((vNormalDesc.r + vNormalDesc.g) / 2 > (vNormalDesc.b * 2.f))
		Out.vSpecGlow = float4(vDiffuse.xyz, 1.f);

	Out.vGlow = float4(0.f, 0.f, 0.f, 1.f);

	Out.vShaderInfo = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_GlowModel(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.99f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	if ((vNormalDesc.r + vNormalDesc.g) / 2 > (vNormalDesc.b * 2.f))
		Out.vGlow = float4(vMtrlDiffuse.xyz, 1.f);

	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_OUTLINE PS_GlowModel_VTF(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;
	vMtrlDiffuse.a = 1.f;

	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.99f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	//if ((vNormalDesc.r + vNormalDesc.g) / 2 > (vNormalDesc.b * 2.f))
	//	Out.vSpecGlow = float4(vMtrlDiffuse.xyz, 1.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	if ((vNormalDesc.r + vNormalDesc.g) / 2 > (vNormalDesc.b * 2.f))
		Out.vGlow = float4(vMtrlDiffuse.xyz, 1.f);

	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

//Dissolve
PS_OUT_OUTLINE PS_Dissolve(PS_IN In)
{
	PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE)0;

	float dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;
	dissolve = dissolve * 0.999f;
	float isVisible = dissolve - g_fDissolveAmount;
	clip(isVisible);
	
	float isGlowing = smoothstep(g_fGlowRange + g_fGlowFalloff, g_fGlowRange, isVisible);
	float3 vDessolveColor = isGlowing * g_vDessolveColor;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	vMtrlDiffuse.a = 1.f;

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.5f, 1.f);

	Out.vOutNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

	if (vNormalDesc.g > vNormalDesc.b)
		Out.vSpecGlow = float4(vMtrlDiffuse.xyz, 1.f);

	Out.vGlow = float4(0.f, 0.f, 0.f, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	Out.vDiffuse += float4(vDessolveColor.xyz, 1.f);
	Out.vGlow = float4(vDessolveColor.xyz, 1.f);
	Out.vShaderInfo = float4(0.9f, 0.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default_0
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

	pass Outline_1
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

	pass Shadow_2
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

	pass VTF_Default_3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass VTF_OutLine_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Outline();
	}

	pass VTF_Shadow_5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

	pass VTF_Eye_6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Eye();
	}
	
	pass VTF_Eye_Burst_7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Eye_Burst();
	}

	pass RimLight_Pass8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RimLight();
	}

	pass DiffuseAddRim_Pass9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DiffuseAddRim();
	}

	pass VTF_GlowModel_Pass10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GlowModel();
	}

	pass VTF_GlowModel_VTF_Pass11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GlowModel_VTF();
	}

	// Dissolve
	pass Dissolve_Model_Pass12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Dissolve();
	}

	pass Dissolve_VTF_Pass13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VTF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Dissolve();
	}

}