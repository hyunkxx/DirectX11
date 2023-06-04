
  
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
float  g_TRadianAcc;
float  g_ERadianAcc;
float  g_RRadianAcc;
float  g_QRadianAcc;
float  g_SkillRadian;
float2  g_GraphUV;
float  g_MonsterGauge;
float2 g_TitleUV;

//Sprite Image
float  g_CurrentCount;		//현재 인덱스
float2 g_SpriteXY;			//가로 세로 갯수

texture2D			g_MyTexture;
texture2D			g_MyTexture2;
texture2D			g_GMask;
texture2D			g_GMask2;
texture2D			g_PMask;
texture2D			g_PMask2;
texture2D			g_Title;
texture2D			g_Sprite;

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

PS_OUT PS_Sprite(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 uv = In.vTexUV;
	float2 SpriteCount = g_SpriteXY; // 가로세로 이미지 개수 5,3

	float fHeight = floor(g_CurrentCount / g_SpriteXY.x);
	float fWidth = frac(g_CurrentCount / g_SpriteXY.x);

	uv.x = In.vTexUV.x / g_SpriteXY.x + fWidth;
	uv.y = In.vTexUV.y / g_SpriteXY.y + (fHeight * (1 / g_SpriteXY.y));

	Out.vColor = g_Sprite.Sample(PointSampler, uv);
//	Out.vColor = Out.vColor * float4(g_vColor, Out.vColor.a);



   	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;
	return Out;
}

PS_OUT PS_COOLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_MyTexture.Sample(PointSampler, In.vTexUV);
	
	Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b);
	
	if(0.9 < Out.vColor.r + Out.vColor.g + Out.vColor.b)
	{
		Out.vColor.a = 0.5f;  
	}

	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;
	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MyTexture.Sample(PointSampler, In.vTexUV);
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

PS_OUT PS_LinearDiscard(PS_IN In)
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
	vector Defuse ,Mask, Mask2;
	Mask = g_PMask.Sample(LinearSampler,  float2(In.vTexUV.x + g_UV.x  ,In.vTexUV.y)); // 흐르는 이미지
	Mask2 = g_PMask2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y)); // 랜더부분 이미지
	Defuse = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	
	Defuse *= (Mask.a +  Mask2.a);


	Out.vColor.a = Defuse;
	Out.vColor.a*=In.vTexUV.x;

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
	
	//float4 fcolor, fDefuse;
	//fcolor = g_MyTexture2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	//fDefuse = g_MyTexture.Sample(LinearSampler,  g_fUV);
	//Out.vColor = fDefuse * fcolor;
	Out.vColor =g_MyTexture2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));

	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	if(Out.vColor.a < 0.1f)
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


   
   	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;
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

   	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

   Out.vColor = vMtrlDiffuse;
   if(Out.vColor.a < 0.1)
	discard;

   return Out;

}


PS_OUT PS_TWO(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 fcolor;
	fcolor = g_MyTexture2.Sample(PointSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	Out.vColor = g_MyTexture.Sample(PointSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	
	if(0.5f > (fcolor.r + fcolor.g + fcolor.b))	
	{
		Out.vColor.a = 0.f;
	}
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	  if(Out.vColor.a < 0.1)
	discard;
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
	Mask = g_GMask.Sample(LinearSampler,  float2(In.vTexUV.x - g_GraphUV.x  ,In.vTexUV.y)); // 흐르는 이미지
	Mask2 = g_GMask2.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y)); // 랜더부분 이미지
	Defuse = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	
	Defuse *= (Mask.a +  Mask2.a);


	Out.vColor.a = Defuse;
	
	Out.vColor.r += g_fColorR/255.f;
	Out.vColor.g += g_fColorG/255.f;
	Out.vColor.b += g_fColorB/255.f;
	Out.vColor.a += g_fColorA/255.f;

	if(In.vTexUV.x > g_SkillRadian)
	   discard;
	if(Out.vColor.a < 0.1)
	discard;
	return Out;

}

PS_OUT PS_COOLEFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector Defuse;
	Defuse = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	float fColor = Defuse.r + Defuse.g + Defuse.b;
	if(0.1f >fColor)
	{ 
	Defuse.a *= fColor;	
	}

	Defuse.r += g_fColorR/255.f;
	Defuse.g += g_fColorG/255.f;
	Defuse.b += g_fColorB/255.f;
	Defuse.a += g_fColorA/255.f;
	 
	 Out.vColor = Defuse;

	return Out;

}

PS_OUT PS_TITLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float4 Defuse = g_MyTexture.Sample(LinearSampler,  float2(In.vTexUV.x ,In.vTexUV.y));
	float4 Mask = g_Title.Sample(LinearSampler,  float2(In.vTexUV.x + g_TitleUV.x ,In.vTexUV.y));
	
	Defuse.r += Mask.r;
	Defuse.g += Mask.g;
	Defuse.b += Mask.b;


	Defuse.r += g_fColorR/255.f;
	Defuse.g += g_fColorG/255.f;
	Defuse.b += g_fColorB/255.f;
	Defuse.a += g_fColorA/255.f;
	Out.vColor = Defuse;

	return Out;
}

PS_OUT PS_MAIN3(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	vector vcolor;
	vcolor = g_MyTexture.Sample(LinearSampler, In.vTexUV);
	if((vcolor.a >0.1f)&&(0.5f > vcolor.r * vcolor.g * vcolor.b))
	{
		vcolor.r += g_fColorR/255.f;
		vcolor.g += g_fColorG/255.f;
		vcolor.b += g_fColorB/255.f;
		vcolor.a += g_fColorA/255.f;
	}
	
	Out.vColor = vcolor;
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
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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

		pass UI_WAVE //6 미니플레이어 그래프마스크 -> 나중에 왼쪽으로 갈수록 연하게 기능 추가할 것
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

	pass UI_MASK // 10 플레이어 메인그래프인데 몬스터도 쓰고 있음 -> 나중에 오른쪽에서 짤리는 기능 추가할것
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

		pass UI_TWO // 14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TWO();
	}

		pass UI_COOLENDEFFECT // 15
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_COOLEFFECT();
	}

			pass UI_MAIN3 // 16
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);        

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN3();
	}

			pass UI_LinearDiscard // 17
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LinearDiscard();
	}

				pass UI_TITLE // 18
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TITLE();
	}

	pass Out_Sprite // 19 
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Sprite();
	}
}
	
	

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