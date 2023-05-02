#include "Shader_Defines.hpp"

matrix				g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D			g_DiffuseTexture;

vector				g_vCamPosition;

float				g_fAlpha;
float3				g_vParticleColor;


float2				g_vUV;
float2				g_vFrame;

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vPSize : PSIZE;

	float4			vRight : TEXCOORD1;
	float4			vUp : TEXCOORD2;
	float4			vLook : TEXCOORD3;
	float4			vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4			vPosition : POSITION;
	float2			vPSize : PSIZE;
	float4			vRight : TEXCOORD1;
	float4			vUp : TEXCOORD2;
	float4			vLook : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = In.vPSize;

	Out.vRight = mul(In.vRight, g_WorldMatrix);
	Out.vUp = mul(In.vUp, g_WorldMatrix);
	Out.vLook = mul(In.vLook, g_WorldMatrix);

	return Out;
}

struct GS_IN
{
	float4			vPosition : POSITION;
	float2			vPSize : PSIZE;
	float4			vRight : TEXCOORD1;
	float4			vUp : TEXCOORD2;
	float4			vLook : TEXCOORD3;
};
struct GS_OUT
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
	float2			vFrame: TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN_SPARK_RHOMBUS(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	int iRandum = length(In[0].vLook);
	float2 vFrame = g_vFrame;

	for (int i = 0; iRandum > i; i++)
	{
		vFrame.x += 1.f;

		if (g_vUV.x <= vFrame.x)
		{
			vFrame.x = 0.f;
			vFrame.y += 1.f;

			if (g_vUV.y <= vFrame.y)
			{
				vFrame.y = 0.f;
			}
		}
	}

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3 vRight, vUp;

	vRight = In[0].vLook / 2.f;
	vUp = In[0].vUp / 2.f;

	Out[0].vPosition = vector(In[0].vPosition.xyz + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.0f, 0.f);
	Out[0].vFrame = vFrame;

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.0f, 0.f);
	Out[1].vFrame = vFrame;

	Out[2].vPosition = vector(In[0].vPosition.xyz - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.0f, 1.f);
	Out[2].vFrame = vFrame;

	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.0f, 1.f);
	Out[3].vFrame = vFrame;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_SPARK_RECT(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	int iRandum = length(In[0].vLook);
	float2 vFrame = g_vFrame;

	for (int i = 0; iRandum > i; i++)
	{
		vFrame.x += 1.f;

		if (g_vUV.x <= vFrame.x)
		{
			vFrame.x = 0.f;
			vFrame.y += 1.f;

			if (g_vUV.y <= vFrame.y)
			{
				vFrame.y = 0.f;
			}
		}
	}

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3 vRight, vUp;

	vRight = In[0].vLook / 2.f;
	vUp = In[0].vUp / 2.f;

	Out[0].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.0f, 0.f);
	Out[0].vFrame = vFrame;

	Out[1].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.0f, 0.f);
	Out[1].vFrame = vFrame;

	Out[2].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.0f, 1.f);
	Out[2].vFrame = vFrame;

	Out[3].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.0f, 1.f);
	Out[3].vFrame = vFrame;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_SPARK_TRIANGLE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	int iRandum = length(In[0].vLook);
	float2 vFrame = g_vFrame;

	for (int i = 0; iRandum > i; i++)
	{
		vFrame.x += 1.f;

		if (g_vUV.x <= vFrame.x)
		{
			vFrame.x = 0.f;
			vFrame.y += 1.f;

			if (g_vUV.y <= vFrame.y)
			{
				vFrame.y = 0.f;
			}
		}
	}

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3 vRight, vUp;

	vRight = In[0].vLook / 2.f;
	vUp = In[0].vUp / 2.f;

	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.0f, 0.f);
	Out[0].vFrame = vFrame;

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.0f, 0.f);
	Out[1].vFrame = vFrame;

	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.0f, 1.f);
	Out[2].vFrame = vFrame;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

}

[maxvertexcount(6)]
void GS_MAIN_BILLBOARD_RHOMBUS(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	int iRandum = length(In[0].vLook);
	float2 vFrame = g_vFrame;

	for (int i = 0; iRandum > i; i++)
	{
		vFrame.x += 1.f;

		if (g_vUV.x <= vFrame.x)
		{
			vFrame.x = 0.f;
			vFrame.y += 1.f;

			if (g_vUV.y <= vFrame.y)
			{
				vFrame.y = 0.f;
			}
		}
	}

	float3 RDist = In[0].vRight.xyz;
	float3 UDist = In[0].vUp.xyz;
	float fRightDist = length(RDist);
	float fUpDist = length(UDist);

	vector		vLook = g_vCamPosition - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * fRightDist * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * fUpDist * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = vector(In[0].vPosition.xyz + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.0f, 0.f);
	Out[0].vFrame = vFrame;

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.0f, 0.f);
	Out[1].vFrame = vFrame;

	Out[2].vPosition = vector(In[0].vPosition.xyz - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.0f, 1.f);
	Out[2].vFrame = vFrame;

	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.0f, 1.f);
	Out[3].vFrame = vFrame;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_BILLBOARD_RECT(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	int iRandum = length(In[0].vLook);
	float2 vFrame = g_vFrame;

	for (int i = 0; iRandum > i; i++)
	{
		vFrame.x += 1.f;

		if (g_vUV.x <= vFrame.x)
		{
			vFrame.x = 0.f;
			vFrame.y += 1.f;

			if (g_vUV.y <= vFrame.y)
			{
				vFrame.y = 0.f;
			}
		}
	}

	float3 RDist = In[0].vRight.xyz;
	float3 UDist = In[0].vUp.xyz;
	float fRightDist = length(RDist);
	float fUpDist = length(UDist);

	vector		vLook = g_vCamPosition - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * fRightDist  * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * fUpDist * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.0f, 0.f);
	Out[0].vFrame = vFrame;

	Out[1].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.0f, 0.f);
	Out[1].vFrame = vFrame;

	Out[2].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.0f, 1.f);
	Out[2].vFrame = vFrame;

	Out[3].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(0.0f, 1.f);
	Out[3].vFrame = vFrame;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

[maxvertexcount(3)]
void GS_MAIN_BILLBOARD_TRIANGLE(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	int iRandum = length(In[0].vLook);
	float2 vFrame = g_vFrame;

	for (int i = 0; iRandum > i; i++)
	{
		vFrame.x += 1.f;

		if (g_vUV.x <= vFrame.x)
		{
			vFrame.x = 0.f;
			vFrame.y += 1.f;

			if (g_vUV.y <= vFrame.y)
			{
				vFrame.y = 0.f;
			}
		}
	}

	float3 RDist = In[0].vRight.xyz;
	float3 UDist = In[0].vUp.xyz;
	float fRightDist = length(RDist);
	float fUpDist = length(UDist);

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vector		vLook = g_vCamPosition - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * fRightDist * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * fUpDist * 0.5f;

	Out[0].vPosition = vector(In[0].vPosition.xyz  + vUp, 1.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(0.0f, 0.f);
	Out[0].vFrame = vFrame;

	Out[1].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(1.0f, 0.f);
	Out[1].vFrame = vFrame;

	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(1.0f, 1.f);
	Out[2].vFrame = vFrame;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();
}


struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
	float2			vFrame: TEXCOORD1;
};

struct PS_OUT
{
	float4			vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor.rgb = g_vParticleColor;

	float2 vDist = float2(0.5f, 0.5f);
	float2 vLength = In.vTexUV - vDist;

	float fDist = sqrt((vLength.x * vLength.x) + (vLength.y * vLength.y));
	
	Out.vColor.a =  1.f - fDist;

	return Out;
}

PS_OUT PS_MAIN_TEX(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 UVSize = 1.f / g_vUV;
	float2 UV = In.vTexUV * UVSize;
	UV += (UVSize * In.vFrame);

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, UV);

	Out.vColor.rgb *= g_vParticleColor;

	return Out;
}

PS_OUT PS_MAIN_TEX_COL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 UVSize = 1.f / g_vUV;
	float2 UV = In.vTexUV * UVSize;
	UV += (UVSize *  In.vFrame);

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, UV);
	
	return Out;
}

PS_OUT PS_MAIN_TEX_ATOC(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 UVSize = 1.f / g_vUV;
	float2 UV = In.vTexUV * UVSize;
	UV += (UVSize * In.vFrame);

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, UV);

	Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;

	Out.vColor.rgb *= g_vParticleColor;

	return Out;
}

PS_OUT PS_MAIN_TEX_COL_ATOC(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 UVSize = 1.f / g_vUV;
	float2 UV = In.vTexUV * UVSize;
	UV += (UVSize * In.vFrame);

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, UV);

	Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;

	return Out;
}


technique11 DefaultTechnique
{
	pass Particle_Spark_Rhombus
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_RHOMBUS();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Particle_Spark_Rect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Particle_Spark_TriAngle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_TRIANGLE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Particle_Billboard_Rhombus
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RHOMBUS();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Particle_Billboard_Rect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Particle_Billboard_TriAngle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_TRIANGLE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	

	pass Particle_Spark_Texture
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX();
	}

	pass Particle_Billboard_Texture
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX();
	}

	pass Particle_Billboard_Sprite
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX();
	}


	pass Particle_Spark_Texture_Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_COL();
	}

	pass Particle_Billboard_Texture_Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_COL();
	}

	pass Particle_Billboard_Sprite_Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_COL();
	}


	pass Particle_Spark_Texture_ATOC
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_ATOC();
	}

	pass Particle_Billboard_Texture_ATOC
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_ATOC();
	}

	pass Particle_Billboard_Sprite_ATOC
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_ATOC();
	}


	pass Particle_Spark_Texture_Color_ATOC
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_SPARK_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_COL_ATOC();
	}

	pass Particle_Billboard_Texture_Color_ATOC
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_COL_ATOC();
	}

	pass Particle_Billboard_Sprite_Color_ATOC
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		//ALPHABLENDENABLE = TRUE;
		//SrcBlend = SrcAlpha;
		//DestBlend = INvSrcAlpha;
		//BlendOp = Add;

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD_RECT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX_COL_ATOC();
	}
}




