#include "SHADER_DEFINES.hpp"

float4x4  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_LightViewMatrix, g_LightProjMatrix;
float4x4 g_BakeLightViewMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;

texture2D g_ShadowDepthTexture;
texture2D g_ShadowTexture;

texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_LightPosTexture;

texture2D g_OutNormalTexture;
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

float fWeight[13] =
{
	0.0561, 0.1353, 0.278, 0.4868, 0.7271, 0.9231, 1,
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
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

struct PS_OUT_SHADOW
{
	float4 vDynamicShadow : SV_TARGET0;
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

	/* 투영스페이스 상의 위치. (로컬정점위치 * 월드 * 뷰 * 투영 / w  */
	/* w = n ~ f */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /*0 ~ 1*/
	vWorldPos.w = 1.f;

	/* 로컬정점위치 * 월드 * 뷰 * 투영 */
	vWorldPos = vWorldPos * (vDepthDesc.y * g_Far);

	/* XMVector3TransformCoord */
	/* 로컬정점위치 * 월드 * 뷰 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* 로컬정점위치 * 월드 */
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

	/* 투영스페이스 상의 위치. (로컬정점위치 * 월드 * 뷰 * 투영 / w  */
	/* w = n ~ f */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /*0 ~ 1*/
	vWorldPos.w = 1.f;

	/* 로컬정점위치 * 월드 * 뷰 * 투영 */
	vWorldPos = vWorldPos * (vDepthDesc.y * g_Far);

	/* XMVector3TransformCoord */
	/* 로컬정점위치 * 월드 * 뷰 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	/* 로컬정점위치 * 월드 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
		   
	vector vLightDir = vWorldPos - g_vLightPos;
	float  fDistance = length(vLightDir);

	/* 빛과 픽셀의 위치가 가까우면 1에가까운값으로 */
	/* range에가까지질수록 0에 가까운값으로 .*/
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
	
	vector vWorldPos;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector vOutNormal = g_OutNormalTexture.Sample(LinearSampler, In.vTexUV);
	vector vShadow = g_ShadowTexture.Sample(LinearSampler, In.vTexUV);

	float4 vFinalColor = (vDiffuse * (vShade * 2.f));
	if (vOutNormal.a == 1.f)
	{
		float vOutline = g_OutlineTexture.Sample(PointSampler, In.vTexUV).r;
		Out.vColor = float4(vFinalColor.xyz * vOutline, vFinalColor.z) * vShadow.r;
	}
	else
	{
		Out.vColor = vFinalColor * vShadow.r;
	}

	if (0.0f == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT_SHADOW PS_Shadow(PS_IN In)
{
	PS_OUT_SHADOW Out = (PS_OUT_SHADOW)0;

	vector vWorldPos;

	vector vDepthInfo = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);
	float fViewZ = vDepthInfo.y * g_Far;

	vector vPosition;
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.x * fViewZ;
	vPosition.w = fViewZ;

	//해당 픽셀의 포지션을 월드까지 내림
	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	vPosition = mul(vPosition, g_LightViewMatrix);

	vector vLightUVPos = mul(vPosition, g_LightProjMatrix);
	float2 vLightUV, vBakeLightUV;

	vLightUV.x = (vLightUVPos.x / vLightUVPos.w) * 0.5f + 0.5f;
	vLightUV.y = (vLightUVPos.y / vLightUVPos.w) * -0.5f + 0.5f;

	vector vShadowDepthInfo = g_ShadowDepthTexture.Sample(LinearBorderSampler, vLightUV);
	
	//깊이 0.5 : 모델과 애님모델에는 그림자 안그림
	if (vPosition.z - 0.05f > (vShadowDepthInfo.g * g_Far) && vDepthInfo.b != vShadowDepthInfo.b)
	{
		Out.vDynamicShadow = vector(0.5f, 0.5f, 0.5f, 0.5f);
	}
	else
	{
		Out.vDynamicShadow = vector(1.f, 1.f, 1.f, 1.f);
	}

	return Out;
}

PS_OUT PS_Outline(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDepthInfo = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector vNormal = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	float4 vColor = 0;
	for (uint i = 0; i < 9; ++i)
		vColor += mask[i] * g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + float2(coord[i % 3] / 1280, coord[i % 3] / 720));

	float fGray = 1.f - (vColor.r * 0.9f + vColor.g * 0.59f + vColor.b * 0.11f);
	float4 Ret = float4(fGray + 0.7f, fGray + 0.7f, fGray + 0.7f, 1) / 1;

	Out.vColor = smoothstep(0.5, 1, Ret);

	return Out;
}

technique11 DefaultTechnique
{
	pass Debug_Pass0
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

	pass Light_Directional_Pass1
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

	pass Light_Point_Pass2
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

	pass Blend_Pass3
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

	pass Outline_Pass4
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

	pass Shadow_Pass5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Shadow();
	}
}