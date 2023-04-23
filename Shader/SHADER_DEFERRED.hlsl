#include "SHADER_DEFINES.hpp"

matrix	  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_OutlineTexture;
		  
vector	  g_vCamPosition;
		  
vector	  g_vLightDir;
vector	  g_vLightPos;
float	  g_fLightRange;
		  
vector	  g_vLightDiffuse;
vector	  g_vLightAmbient;
vector	  g_vLightSpecular;
		  
vector	  g_vMtrlAmbient = vector(0.8f, 0.8f, 0.8f, 1.f);
vector	  g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
		  
float4x4  g_ProjMatrixInv;
float4x4  g_ViewMatrixInv;

float mask[9] =
{
	-1.f, -1.f, -1.f,
	-1.f,  8.f, -1.f,
	-1.f, -1.f, -1.f
};

float coord[3] = { -1.f, 0.f, 1.f };

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

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

struct PS_OUT_LIGHT
{
	float4 vShade : SV_TARGET0;
	float4 vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
		   
	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	//Toon
	float fDot = max(0, dot(vNormal, -g_vLightDir));
	if (fDot < 0.2)
		fDot = 0.93f;
	else
		fDot = 1.0f;

	vector vShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)));

	Out.vShade = g_vLightDiffuse * fDot * saturate(vShade + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector			vWorldPos;

	/* ���������̽� ���� ��ġ. (����������ġ * ���� * �� * ���� / w  */
	/* w = n ~ f */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /*0 ~ 1*/
	vWorldPos.w = 1.f;

	/* ����������ġ * ���� * �� * ���� */
	vWorldPos = vWorldPos * (vDepthDesc.y * 1000.f);

	/* XMVector3TransformCoord */
	/* ����������ġ * ���� * �� */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* ����������ġ * ���� */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector vLook = vWorldPos - g_vCamPosition;
		   
	float  fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
		   
	vector vWorldPos;

	/* ���������̽� ���� ��ġ. (����������ġ * ���� * �� * ���� / w  */
	/* w = n ~ f */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /*0 ~ 1*/
	vWorldPos.w = 1.f;

	/* ����������ġ * ���� * �� * ���� */
	vWorldPos = vWorldPos * (vDepthDesc.y * 300.f);

	/* XMVector3TransformCoord */
	/* ����������ġ * ���� * �� */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* ����������ġ * ���� */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
		   
	vector vLightDir = vWorldPos - g_vLightPos;
	float  fDistance = length(vLightDir);

	/* ���� �ȼ��� ��ġ�� ������ 1���������� */
	/* range�������������� 0�� �������� .*/
	float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

	vector vShade = saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal)));


	Out.vShade = g_vLightDiffuse * saturate(vShade + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	Out.vShade.a = 1.f;

	vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector vLook = vWorldPos - g_vCamPosition;
		   
	float  fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular * fAtt;
	Out.vSpecular.a = 0.f;


	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	vector vOutline = g_OutlineTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor = (vDiffuse * (vShade * 2.f)) * vOutline;

	if (0.0f == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_Outline(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDepthInfo = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormal = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float4 vColor = 0;
	for (int i = 0; i < 9; ++i)
		vColor += mask[i] * g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + float2(coord[i % 3] / 1280, coord[i % 3] / 720));

	float fGray = 1.f - (vColor.r * 0.3f + vColor.g * 0.59f + vColor.b * 0.11f);
	float4 Ret = float4(fGray + 0.3f, fGray + 0.3f, fGray + 0.3f, 1) / 1;

	Out.vColor = smoothstep(0.8, 1, Ret);

	return Out;
}

technique11 DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	pass Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Outline();
	}
}