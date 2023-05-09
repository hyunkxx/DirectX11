#include "Shader_Defines.hpp"

matrix				g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix				g_InvWorldMatrix, g_InvViewMatrix, g_InvProjMatrix;

matrix				g_InvWV;
float4				g_InvProj;

texture2D			g_DiffuseTexture;
texture2D			g_NormalTexture;
texture2D			g_DepthTexture;
texture2D			g_Static_DepthTexture;

texture2D			g_BackTexture;
texture2D			g_BackNormalTexture;

float4		g_vCamPosition;
float3		g_vColor;

float2		g_vUV;
float		g_fAlpha;
float		g_fDisPower;

float		g_ihlslWinSizeX = 1280.f;
float		g_ihlslWinSizeY = 720.f;

float		g_fTimeAcc;


float4 Get_Local(float2 vUV, float4 vDepth)
{
	float3 vProj = float3(lerp(-g_InvProj.xy, g_InvProj.xy, vUV), g_InvProj.z);
	float fViewZ = vDepth.y * g_Far;

	float4 vPos = float4(vProj * fViewZ, 1.f);
	float4 vLocalpos = mul(vPos, g_InvWV);

	return vLocalpos;
}

struct VS_IN
{
	float3			vPosition : POSITION;
	float3			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
	float3			vTangent : TANGENT;
};

struct VS_IN_CUBE
{
	float3			vPosition : POSITION;
	float3			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct VS_OUT_NORMAL
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

	float3		vTangent : TANGENT;
	float3		vBiNormal : BINORMAL;

};

struct VS_OUT_DECAL
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vRay : TEXCOORD3;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_NORMAL VS_MAIN_NORMALTEX(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBiNormal = normalize(cross(Out.vNormal, Out.vTangent));

	return Out;
}

VS_OUT_DECAL VS_MAIN_DECAL(VS_IN_CUBE In)
{
	VS_OUT_DECAL		Out = (VS_OUT_DECAL)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vRay = mul(float4(In.vPosition, 1.f), matWV);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_IN_NORMAL
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

	float3		vTangent : TANGENT;
	float3		vBiNormal : BINORMAL;
};

struct PS_IN_DECAL
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vRay : TEXCOORD3;
};

struct PS_OUT
{
	float4			vDiffuse : SV_TARGET0;
	float4			vNormal : SV_TARGET1;
	float4			vDepth : SV_TARGET2;
};

struct PS_OUT_DECAL
{
	float4			vDiffuse : SV_TARGET0;
	float4			vNormal : SV_TARGET1;
};

struct PS_OUT_DECAL_NONORMAL
{
	float4			vDiffuse : SV_TARGET0;
};


struct PS_OUT_GLOW
{
	float4			vDiffuse : SV_TARGET0;
	float4			vDistortion : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	Out.vDiffuse = vDiffuseColor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_NORMALTEX(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vDiffuseColor;
	Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);

	return Out;
}

PS_OUT_GLOW PS_MAIN_PARTICLE_CORLOR(PS_IN In)
{
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;;

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

PS_OUT_GLOW PS_MAIN_PARTICLE_CORLOR_CLAMP(PS_IN In)
{
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;;

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

PS_OUT_GLOW PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;;

	float2 vUV = In.vTexUV + g_vUV;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;

	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT_GLOW PS_MAIN_ALPHA_CLAMP(PS_IN In)
{
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;

	float2 vUV = clamp(In.vTexUV + g_vUV, 0.f, 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;
	Out.vDiffuse.a = saturate(vMtrlDiffuse.a - (1.f - g_fAlpha));
	Out.vDistortion = float4(0.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_DISTORTION(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV + g_vUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;

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
	float3 vWorldPos = In.vWorldPos + (normalize(vNormal) * g_fDisPower);

	matrix matVP;
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float4 vProjPos = mul(float4(vWorldPos, 1.f), matVP);

	float2 vUV;
	vUV.x = saturate((vProjPos.x / vProjPos.w) * 0.5f + 0.5f);
	vUV.y = saturate((vProjPos.y / vProjPos.w) * -0.5f + 0.5f);

	//float4 vDepth = g_DepthTexture.Sample(LinearClampSampler, vUV);


	Out.vDiffuse = g_BackTexture.Sample(LinearClampSampler, vUV);
	Out.vDiffuse.a = 1.f;

	return Out;
}

PS_OUT	PS_MAIN_DISTORTION_N0_DEPTH(PS_IN_NORMAL In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV + g_vUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;

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

	float3 vWorldPos = In.vWorldPos + (normalize(vNormal) * g_fDisPower);

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

PS_OUT_DECAL	PS_MAIN_DECAL(PS_IN_DECAL In)
{
	PS_OUT_DECAL			Out = (PS_OUT_DECAL)0;

	float2 vUV;
	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepth = g_DepthTexture.Sample(LinearSampler, vUV);
	float4 vStaitc_Depth = g_Static_DepthTexture.Sample(LinearSampler, vUV);

	if (0.f == vDepth.a)
		discard;

	if (vStaitc_Depth.r != vDepth.r || vStaitc_Depth.g != vDepth.g)
		discard;

	float4 vLocalpos = Get_Local(vUV, vDepth);

	clip(0.5 - abs(vLocalpos.xyz));

	float2 decalUV = vLocalpos.xz + 0.5f;
	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, decalUV);
	Out.vDiffuse.a = saturate(Out.vDiffuse.a - (1.f - g_fAlpha));

	return Out;
}

PS_OUT_DECAL	PS_MAIN_DECAL_NORMAL(PS_IN_DECAL In)
{
	PS_OUT_DECAL			Out = (PS_OUT_DECAL)0;

	float2 vUV;
	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepth = g_DepthTexture.Sample(LinearSampler, vUV);
	float4 vStaitc_Depth = g_Static_DepthTexture.Sample(LinearSampler, vUV);

	if (0.f == vDepth.a)
		discard;

	if (vStaitc_Depth.r != vDepth.r || vStaitc_Depth.g != vDepth.g)
		discard;

	float4 vLocalpos = Get_Local(vUV, vDepth);
	clip(0.5 - abs(vLocalpos.xyz));

	float2 decalUV = vLocalpos.xz + 0.5f;
	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, decalUV);
	Out.vDiffuse.a = saturate(Out.vDiffuse.a - (1.f - g_fAlpha));

	//if (0.f == Out.vDiffuse.a)
	//discard;

	float4 vScreenNormal = g_BackNormalTexture.Sample(LinearClampSampler, vUV);
	vScreenNormal.xyz = vScreenNormal.xyz * 2.f - 1.f;
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, decalUV);
	float3	 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3 vLook;

	if (vScreenNormal.y > 0.f)
		vLook = (0.f, 0.f, 1.f);
	else
		vLook = (0.f, 0.f, -1.f);

	float3 vTangent;
	float3 vBiNormal;

	vTangent = normalize(cross(vScreenNormal, vLook));
	vBiNormal = normalize(cross(vTangent, vScreenNormal));

	float3x3 WorldMatrix = float3x3(vTangent, vBiNormal, vScreenNormal.xyz);
	Out.vNormal = float4(normalize(mul(vNormal, WorldMatrix)), 1.f);

	return Out;
}

PS_OUT_DECAL_NONORMAL	PS_MAIN_DECAL_COLOR_ALPHA(PS_IN_DECAL In)
{
	PS_OUT_DECAL_NONORMAL			Out = (PS_OUT_DECAL_NONORMAL)0;

	float2 vUV;
	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepth = g_DepthTexture.Sample(LinearSampler, vUV);
	float4 vStaitc_Depth = g_Static_DepthTexture.Sample(LinearSampler, vUV);

	if (0.f == vDepth.a)
		discard;

	if (vStaitc_Depth.r != vDepth.r || vStaitc_Depth.g != vDepth.g)
		discard;

	float4 vLocalpos = Get_Local(vUV, vDepth);

	clip(0.5 - abs(vLocalpos.xyz));

	float2 decalUV = vLocalpos.xz + 0.5f;
	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, decalUV);
	Out.vDiffuse.xyz = Out.vDiffuse.xyz * g_vColor.xyz;
	Out.vDiffuse.a = saturate(Out.vDiffuse.a - (1.f - g_fAlpha));

	return Out;
}

PS_OUT_DECAL_NONORMAL	PS_MAIN_DECAL_COLOR(PS_IN_DECAL In)
{
	PS_OUT_DECAL_NONORMAL			Out = (PS_OUT_DECAL_NONORMAL)0;

	float2 vUV;
	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepth = g_DepthTexture.Sample(LinearSampler, vUV);
	float4 vStaitc_Depth = g_Static_DepthTexture.Sample(LinearSampler, vUV);

	if (0.f == vDepth.a)
		discard;

	if (vStaitc_Depth.r != vDepth.r || vStaitc_Depth.g != vDepth.g)
		discard;

	float4 vLocalpos = Get_Local(vUV, vDepth);

	clip(0.5 - abs(vLocalpos.xyz));

	float2 decalUV = vLocalpos.xz + 0.5f;
	float4 vColor = g_DiffuseTexture.Sample(LinearSampler, decalUV);
	Out.vDiffuse.a = (vColor.x + vColor.y + vColor.z) / 3.f;
	Out.vDiffuse.xyz = vColor.xyz * g_vColor.xyz;
	Out.vDiffuse.a = saturate(Out.vDiffuse.a - (1.f - g_fAlpha));

	//if (0.f == Out.vDiffuse.a)
	//discard;

	return Out;
}

technique11 DefaultTechnique
{
	/* 픽셀단위에서 빛연산을 수행한다. */
	pass Default_Texture
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_NORMAL
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMALTEX();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMALTEX();
	}

	pass Particle_Color
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE_CORLOR();
	}

	pass Particle_Color_Clamp
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE_CORLOR_CLAMP();
	}

	pass Particle_Alpha
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass Particle_Alpha_Clamp
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_CLAMP();
	}

	pass Distortion
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMALTEX();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	pass Distortion_No_Depth
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMALTEX();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_N0_DEPTH();
	}

	pass Decal
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DECAL();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DECAL();
	}

	pass Decal_Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DECAL();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DECAL_NORMAL();
	}

	pass Decal_Color_Alpha
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DECAL();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DECAL_COLOR_ALPHA();
	}

	pass Decal_Color
	{
		SetRasterizerState(/*RS_Wireframe*/RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DECAL();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DECAL_COLOR();
	}
}