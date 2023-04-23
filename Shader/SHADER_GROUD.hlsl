
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightPos = vector(10.f, 3.f, 10.f, 1.f);
float g_fLength = 10.f;

/* ������ ���� */
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_DiffuseTexture;
vector g_vMatAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMatSpecular = vector(1.f, 1.f, 1.f, 1.f);
float g_fPower = 50.f;

sampler LinearSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state {
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float fShade : TEXCOORD1;
	float fSpecular : TEXCOORD2;
};

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float fShade : TEXCOORD1;
	float fSpecular : TEXCOORD2;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	vector vWorldNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0.f);

	vector vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vector vLook = vWorldPos - g_vCamPosition;

	Out.fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), g_fPower);

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 5.f);
	
	Out.vColor = (g_vLightDiffuse * vMatDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMatAmbient)) + (g_vLightSpecular * g_vMatSpecular) * In.fSpecular;

	return Out;
}

technique11 Default
{
	pass Groud
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};