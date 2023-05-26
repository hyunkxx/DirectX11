#include "Shader_Defines.hpp"

matrix				g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D			g_DiffuseTexture;

texture2D			g_DiffuseTexture_1;
texture2D			g_DiffuseTexture_2;
texture2D			g_DiffuseTexture_3;
texture2D			g_DiffuseTexture_4;

texture2D			g_NormalTexture_1;
texture2D			g_NormalTexture_2;
texture2D			g_NormalTexture_3;
texture2D			g_NormalTexture_4;

float				g_fUVSampler_Ratio_1;
float				g_fUVSampler_Ratio_2;
float				g_fUVSampler_Ratio_3;
float				g_fUVSampler_Ratio_4;

texture2D			g_FilterTexture;

texture2D			g_BrushTexture;
vector				g_vBrushPos = vector(10.f, 0.f, 0.f, 1.f);
float				g_fBrushRange = 5.f;

struct VS_IN
{
	float3			vPosition : POSITION;
	float3			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
	float3			vTangent : TANGENT;
};


struct VS_OUT_PHONG
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

	float3		vTangent : TANGENT;
	float3		vBiNormal : BINORMAL;
};

VS_OUT_PHONG VS_MAIN_PHONG(VS_IN In)
{
	VS_OUT_PHONG		Out = (VS_OUT_PHONG)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent)).xyz;

	return Out;
}

struct PS_IN_PHONG
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

	float3		vTangent : TANGENT;
	float3		vBiNormal : BINORMAL;
};

struct PS_OUT
{
	float4			vDiffuse : SV_TARGET0;
	float4			vNormal : SV_TARGET1;
	float4			vDepth : SV_TARGET2;
	float4			vOutNormal : SV_TARGET3;
	float4			vGlow : SV_TARGET4;
	float4			vSpecGlow : SV_TARGET5;
	float4			vShaderInfo : SV_TARGET6; // r : ���� ���� ���� ���� ����������� ���� ���̴� �Ӽ����� ������ ����
};

/*
PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vSourDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse = vSourDiffuse;

	vector		vBrushColor = vector(0.f, 0.f, 0.f, 0.f);

	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (g_fBrushRange * 2.f),
			((g_vBrushPos.z - g_fBrushRange) - In.vWorldPos.z) / (g_fBrushRange * 2.f));
		vBrushColor = g_BrushTexture.Sample(LinearSampler, vUV);
	}

	Out.vDiffuse = vMtrlDiffuse + vBrushColor;
	Out.vDiffuse.a = 1.f;

	// In.vNormal.xyz => -1 ~ 1 
	// Out.vNormal.xyz => 0 ~ 1 
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	Out.vOutNormal = float4(0.5f, 0.5f, 0.5f, 1.f);

	return Out;
}
*/

PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse_1 = g_DiffuseTexture_1.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDiffuse_2 = g_DiffuseTexture_2.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDiffuse_3 = g_DiffuseTexture_3.Sample(LinearSampler, In.vTexUV * 100.f);
	vector		vDiffuse_4 = g_DiffuseTexture_4.Sample(LinearSampler, In.vTexUV * 100.f);

	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse = { 0.0f, 0.0f, 0.0f, 0.0f };

	float		fTotalFilter = vFilter.r + vFilter.g + vFilter.b + vFilter.a;

	if (0.0f < fTotalFilter)
	{
		vFilter = vFilter / fTotalFilter;

		vMtrlDiffuse = (vDiffuse_1 * vFilter.r)
			+ (vDiffuse_2 * vFilter.g)
			+ (vDiffuse_3 * vFilter.b)
			+ (vDiffuse_4 * (1.0f - (vFilter.r + vFilter.g + vFilter.b)));
	}
	else
		vMtrlDiffuse = vDiffuse_4;

	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a = 1.f;

	// In.vNormal.xyz => -1 ~ 1
	// Out.vNormal.xyz => 0 ~ 1
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);
	Out.vGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vShaderInfo = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_PHONG_NORMALMAP(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse_1 = g_DiffuseTexture_1.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_1);
	vector		vDiffuse_2 = g_DiffuseTexture_2.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_2);
	vector		vDiffuse_3 = g_DiffuseTexture_3.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_3);
	vector		vDiffuse_4 = g_DiffuseTexture_4.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_4);

	vector		vNormalDesc_1 = g_NormalTexture_1.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_1);
	vector		vNormalDesc_2 = g_NormalTexture_2.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_2);
	vector		vNormalDesc_3 = g_NormalTexture_3.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_3);
	vector		vNormalDesc_4 = g_NormalTexture_4.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_4);

	float3		vNormal_1 = vNormalDesc_1.xyz * 2.0f - 1.0f;
	float3		vNormal_2 = vNormalDesc_2.xyz * 2.0f - 1.0f;
	float3		vNormal_3 = vNormalDesc_3.xyz * 2.0f - 1.0f;
	float3		vNormal_4 = vNormalDesc_4.xyz * 2.0f - 1.0f;

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal_1 = mul(vNormal_1, WorldMatrix);
	vNormal_2 = mul(vNormal_2, WorldMatrix);
	vNormal_3 = mul(vNormal_3, WorldMatrix);
	vNormal_4 = mul(vNormal_4, WorldMatrix);

	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
	vector		vNormal = { 0.0f, 0.0f, 0.0f, 0.0f };

	float		fTotalFilter = vFilter.r + vFilter.g + vFilter.b + vFilter.a;

	if (0.0f < fTotalFilter)
	{
		vFilter = vFilter / fTotalFilter;

		vMtrlDiffuse = (vDiffuse_1 * vFilter.r)
			+ (vDiffuse_2 * vFilter.g)
			+ (vDiffuse_3 * vFilter.b)
			+ (vDiffuse_4 * (1.0f - (vFilter.r + vFilter.g + vFilter.b)));

		vNormal = (float4(vNormal_1, 0.0f) * vFilter.r)
			+ (float4(vNormal_2, 0.0f) * vFilter.g)
			+ (float4(vNormal_3, 0.0f) * vFilter.b)
			+ (float4(vNormal_4, 0.0f) * (1.0f - (vFilter.r + vFilter.g + vFilter.b)));
	}
	else
	{
		vMtrlDiffuse = vDiffuse_4;
		vNormal = float4(vNormal_4, 0.0f);
	}

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = 1.f;

	/* In.vNormal.xyz => -1 ~ 1 */
	/* Out.vNormal.xyz => 0 ~ 1 */
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);
	Out.vGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vShaderInfo = float4(0.f, 0.f, 0.f, 0.f);


	return Out;
}


PS_OUT PS_MAIN_PHONG_NORMALMAP_DARK(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse_1 = g_DiffuseTexture_1.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_1);
	vector		vDiffuse_2 = g_DiffuseTexture_2.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_2);
	vector		vDiffuse_3 = g_DiffuseTexture_3.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_3);
	vector		vDiffuse_4 = g_DiffuseTexture_4.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_4);

	vector		vNormalDesc_1 = g_NormalTexture_1.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_1);
	vector		vNormalDesc_2 = g_NormalTexture_2.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_2);
	vector		vNormalDesc_3 = g_NormalTexture_3.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_3);
	vector		vNormalDesc_4 = g_NormalTexture_4.Sample(LinearSampler, In.vTexUV * g_fUVSampler_Ratio_4);

	float3		vNormal_1 = vNormalDesc_1.xyz * 2.0f - 1.0f;
	float3		vNormal_2 = vNormalDesc_2.xyz * 2.0f - 1.0f;
	float3		vNormal_3 = vNormalDesc_3.xyz * 2.0f - 1.0f;
	float3		vNormal_4 = vNormalDesc_4.xyz * 2.0f - 1.0f;

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);

	vNormal_1 = mul(vNormal_1, WorldMatrix);
	vNormal_2 = mul(vNormal_2, WorldMatrix);
	vNormal_3 = mul(vNormal_3, WorldMatrix);
	vNormal_4 = mul(vNormal_4, WorldMatrix);

	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vMtrlDiffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
	vector		vNormal = { 0.0f, 0.0f, 0.0f, 0.0f };

	float		fTotalFilter = vFilter.r + vFilter.g + vFilter.b + vFilter.a;

	if (0.0f < fTotalFilter)
	{
		vFilter = vFilter / fTotalFilter;

		vMtrlDiffuse = (vDiffuse_1 * vFilter.r)
			+ (vDiffuse_2 * vFilter.g)
			+ (vDiffuse_3 * vFilter.b)
			+ (vDiffuse_4 * (1.0f - (vFilter.r + vFilter.g + vFilter.b)));

		vNormal = (float4(vNormal_1, 0.0f) * vFilter.r)
			+ (float4(vNormal_2, 0.0f) * vFilter.g)
			+ (float4(vNormal_3, 0.0f) * vFilter.b)
			+ (float4(vNormal_4, 0.0f) * (1.0f - (vFilter.r + vFilter.g + vFilter.b)));
	}
	else
	{
		vMtrlDiffuse = vDiffuse_4;
		vNormal = float4(vNormal_4, 0.0f);
	}

	Out.vDiffuse = vMtrlDiffuse * 0.5f;
	Out.vDiffuse.a = 1.f;

	/* In.vNormal.xyz => -1 ~ 1 */
	/* Out.vNormal.xyz => 0 ~ 1 */
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.f);
	Out.vGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vShaderInfo = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_FLOOR(PS_IN_PHONG In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 70.f);

	Out.vDiffuse.gb = Out.vDiffuse.r;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
	Out.vOutNormal = float4(0.f, 0.f, 0.f, 0.0f);
	Out.vGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vSpecGlow = float4(0.f, 0.f, 0.f, 0.f);
	Out.vShaderInfo = float4(0.f, 0.f, 0.f, 0.f);
	
	return Out;
}

technique11 DefaultTechnique
{
	/* �ȼ��������� �������� �����Ѵ�. */
	pass Terrain_Phong
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PHONG();
	}

	pass Terrain_Floor
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOOR();
	}

	pass Terrain_Phong_NormalMap
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PHONG_NORMALMAP();
	}

	pass Terrain_Phong_Wireframe
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PHONG();
	}

	pass Terrain_Phong_NormalMap_Dark_Pass4
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_PHONG();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PHONG_NORMALMAP_DARK();
	}
}