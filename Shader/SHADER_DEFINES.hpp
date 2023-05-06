/* For.SamplerState */

float g_Far = 500.f;

#define EPSILON 0.00001

sampler LinearSampler = sampler_state{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearClampSampler = sampler_state{
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

sampler LinearBorderSampler = sampler_state{
	filter = min_mag_mip_linear;
	BorderColor = float4(1.f, 1.f, 1.f, 0.f);
	AddressU = Border;
	AddressV = Border;
};

sampler PointSampler = sampler_state{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

/* For.RasterizerState */
RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_CullFront
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState RS_CullBack
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState	RS_Sky
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_ShadowDepth
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

/* For.DepthStencilState */
DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = All;
	DepthFunc = less_equal;
};

DepthStencilState DS_Not_ZTest_ZWrite
{
	DepthEnable = false;
	DepthWriteMask = Zero;
};

DepthStencilState DS_ZTest_NoZWrite
{
	DepthEnable = true;
	DepthWriteMask = Zero;
};


/* For.BlendState */
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	/* 인덱스 지정 필수 */
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	/* 인덱스 지정 선택 */
	BlendOp   = add;
	SrcBlend  = src_alpha;
	DestBlend = Inv_src_alpha;
};

BlendState BS_OneBlend
{
	/* 인덱스(장치에 바인딩되어있는 렌더타겟의 인덱스) 지정 필수 */
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	/* 인덱스 지정 선택 */
	BlendOp   = add;
	SrcBlend  = one;
	DestBlend = one;
};
