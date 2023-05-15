
#include "Shader_Defines.hpp"

matrix				g_MyWorldMatrix, g_MyViewMatrix, g_MyProjMatrix;

float  g_fColorR;
float  g_fColorG;
float  g_fColorB;
float  g_fColorA;
float  g_fLoading;
float  g_fwhiteBar;
float  g_fRedBar;
float  g_fUV_Y;
float  g_fCoolTime;
float2 g_fLU;
float2 g_fRB;
float2 g_UV;
float  g_RadianAcc;
float2  g_MonUV;
float  g_MonsterGauge;
texture2D			g_MyTexture;
texture2D			g_MyTexture2;
texture2D			g_Mask;
texture2D			g_Mask2;
struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_MyWorldMatrix, g_MyViewMatrix);
	matWVP = mul(matWV, g_MyProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4			vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	if(Out.vColor.a < 0.1)
	discard;

	return Out;
}

PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	return Out;
}

PS_OUT PS_MAIN_LOADING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(PointSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	if(In.vTexUV.x > g_fLoading)
	   discard;
	
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	if(Out.vColor.a < 0.1)
	discard;
	return Out;
}
PS_OUT PS_REDBAR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(PointSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	if(In.vTexUV.x > g_fRedBar)
	   discard;
	
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	return Out;
}


PS_OUT PS_MONHPW(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(PointSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	if(In.vTexUV.x > g_fwhiteBar)
	   discard;
	
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	return Out;
}
PS_OUT PS_MONHPR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(PointSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	if(In.vTexUV.x > g_fRedBar)
	   discard;
	
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	return Out;
}

PS_OUT PS_MAIN_COOLTIME(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float4 fcolor;
	fcolor = g_MyTexture2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	Out.vColor = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	Out.vColor *= fcolor.a;

	if(In.vTexUV.y < g_fCoolTime )
	discard;

	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	return Out;
}
PS_OUT PS_WAVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x + g_UV.x ,In.vTexUV.y + g_UV.y));
	Out.vColor.a = In.vTexUV.x;
	if(Out.vColor.r<0.1f)
	discard;
	if(Out.vColor.g<0.1f)
	discard;
	if(Out.vColor.b<0.1f)
	discard;
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	return Out;
}
PS_OUT PS_CUT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 g_fUV;
	g_fUV.x = (g_fRB.x - g_fLU.x) * In.vTexUV.x + g_fLU.x;
	g_fUV.y = (g_fRB.y - g_fLU.y) * In.vTexUV.y + g_fLU.y;

	Out.vColor = g_MyTexture.Sample(LinearSampler, g_fUV);

	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	if(Out.vColor.a < 0.1)
	discard;

	return Out;
}

PS_OUT PS_CYCLE(PS_IN In)
{
   PS_OUT Out = (PS_OUT)0;;

   vector vMtrlDiffuse = g_MyTexture.Sample(LinearSampler, In.vTexUV);

   float2 vCurDir = In.vTexUV - float2(0.5f, 0.5f);

   float angle = degrees(atan2(vCurDir.x , +vCurDir.y));

   if ((angle + 180.f) / 360.f < (1 - g_RadianAcc))
   {
      vMtrlDiffuse.a -= 0.5f;
      vMtrlDiffuse.a = max(vMtrlDiffuse.a, 0.f);
   }


   Out.vColor = vMtrlDiffuse;

   return Out;

}

PS_OUT PS_CYCLESKILL(PS_IN In)
{
   PS_OUT Out = (PS_OUT)0;;

   vector vMtrlDiffuse = g_MyTexture.Sample(LinearSampler, In.vTexUV);

   float2 vCurDir = In.vTexUV - float2(0.5f, 0.5f);

   float angle = degrees(atan2(-vCurDir.x , +vCurDir.y));

   if ((angle + 180.f) / 360.f < (1 - g_RadianAcc))
   {
      vMtrlDiffuse.a -= 0.5f;
      vMtrlDiffuse.a = max(vMtrlDiffuse.a, 0.f);
   }


   Out.vColor = vMtrlDiffuse;

   return Out;

}


PS_OUT PS_TWO(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 fcolor;
	fcolor = g_MyTexture2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	Out.vColor = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	
	if((fcolor.r >0.5f)&&(fcolor.g >0.5f)&&(fcolor.b >0.5f))	
	Out.vColor.a = 0.f;

	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;


	return Out;
}


PS_OUT PS_DOWN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	if(In.vTexUV.y > g_fUV_Y)
	   discard;
	
 	 Out.vColor.r += g_fUV_Y;
	 Out.vColor.g += g_fUV_Y;
	 Out.vColor.b += g_fUV_Y;
	
	Out.vColor.a += g_fColorA/255.f;
	return Out;
}

PS_OUT PS_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector Defuse ,Mask, Mask2;
	Mask = g_Mask.Sample(LinearSampler,  float2(In.vTexUV.x - g_MonUV.x  ,In.vTexUV.y)); // 흐르는 이미지
	Mask2 = g_Mask2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y)); // 랜더부분 이미지
	Defuse = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	
	Defuse *= (Mask.a +  Mask2.a);


	Out.vColor.a = Defuse;

	return Out;

}

technique11 DefaultTechnique
{

	pass UI //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

		pass UI_Alpha //1 알파discard
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN2();
	}

	
		pass UI_LOADING //2 timeacc받아서 작으면 출력
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LOADING();
	}

			pass UI_Alpha2 //3 알파discard 없음
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN2();
	}

	pass UI_COOLTIME //4 텍스처 두개 받아서 겹치는 부분만 랜더
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COOLTIME();
	}

		pass UI_CUT //5 지정한uv만큼만 출력
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CUT();
	}

		pass UI_WAVE //6 흐르는데 왼쪽으로 갈수록 연하게
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WAVE();
	}

		pass UI_CYCLE //7 
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CYCLE();
	}
			pass UI_REDBAR //8 timeacc받아서 작으면 출력
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_REDBAR();
	}

	pass UI_DOWN //9 
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DOWN();
	}

	pass UI_MASK // 10 -> 얘 지우고 마스크 이미지
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK();
	}

		pass UI_MONHPW //11 체력 흰
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MONHPW();
	}

		pass UI_MONHPR //12 체력 빨
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MONHPR();
	}

			pass UI_CYCLESKILL // 13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CYCLESKILL();
	}

}

	
	//pass Out_File //7 
	//{
	//	SetRasterizerState(RS_Default);
	//	SetDepthStencilState(DS_Default, 0);
	//	SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	GeometryShader = NULL;
	//	HullShader = NULL;
	//	DomainShader = NULL;
	//	PixelShader = compile ps_5_0 PS_MAIN_OUT();
	//}

//PS_OUT PS_MAIN_EDIT(PS_IN In)
//{
//	PS_OUT			Out = (PS_OUT)0;

//	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
//	Out.vColor.rgb *= Out.vColor.a;
//	Out.vColor.a = 1.f;
//	return Out;
//}

//	pass UI_Edit
//	{
//		SetRasterizerState(RS_Default);
//		SetDepthStencilState(DS_Default, 0);
//		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

//		VertexShader = compile vs_5_0 VS_MAIN();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile ps_5_0 PS_MAIN_EDIT();
//	}
	

//struct PS_IN_SOFTEFFECT
//{
//	float4			vPosition : SV_POSITION;
//	float2			vTexUV : TEXCOORD0;
//	float4			vProjPos : TEXCOORD1;
//};

//PS_OUT PS_MAIN_SOFTEFFECT(PS_IN_SOFTEFFECT In)
//{
//	PS_OUT			Out = (PS_OUT)0;

//	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

//	float2		vDepthUV;

//	vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
//	vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

//	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, vDepthUV);

//	float		fOldViewZ = vDepthDesc.y * 300.f;

//	float		fViewZ = In.vProjPos.w;

//	Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ);

//	return Out;
//}