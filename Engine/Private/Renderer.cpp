#include "..\Public\Renderer.h"
#include "GameInstance.h"
#include "TargetManager.h"
#include "RenderTarget.h"
#include "LightManager.h"
#include "PipeLine.h"

#include "GameObject.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

#include "SSAO.h"

#include "RenderSetting.h"

void CRenderer::Draw()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_double TimeDelta = pGameInstance->GetTimeDelta();

	Render_Priority();
	Render_DynamicShadowMap();
	Render_Static();

	Extraction(L"Target_Static_Depth", L"Target_Depth");

	Render_Dynamic();
	Render_SSD();
	Render_SSD_BLEND();

	if (m_pRenderSetting->IsActiveSSAO())
	{
		Ready_SSAO(L"Target_SSAO");
		Target_Blur(L"Target_SSAO");
		Extraction(L"Target_SSAO", L"Target_BlurY");
	}

	Render_Lights();
	Extraction(L"Target_BlackWhite", L"Target_Shade", 1);

	Render_Outline();

	if (m_pRenderSetting->IsActiveShadow() && !m_pRenderSetting->IsActiveBlackWhite())
	{
		Render_Shadow();

		if (m_pRenderSetting->GetShadowLevel() == CRenderSetting::SHADOW_HIGH)
		{
			Target_Blur(L"Target_Shadow");
			Extraction(L"Target_Shadow", L"Target_BlurY");
		}
	}

	if (m_pRenderSetting->GetLUT() != CRenderer::LUT_DEFAULT)
	{
		ApplyLUT(m_pRenderSetting->GetLUT());
		Extraction(L"Target_Diffuse_SSD_Blend", L"Target_LUT");
	}

	Render_GlowSSD();
	Render_Particle();
	Render_SpecularGlow();

	Render_Blend();

	Render_Glow();
	Render_Distortion();
	Render_Glow_Blend();

	AddCombine(L"Target_FinalBlend", L"Target_Final", L"Target_SpecGlow");
	if (m_pRenderSetting->IsActiveRGBSplit())
	{
		RGBSplit(L"Target_Split", L"Target_FinalBlend");
		Extraction(L"Target_FinalBlend", L"Target_Split");
	}

	//Final 백버퍼에 최종 렌더
	FinalExtraction();

	Render_NonLight();
	Render_AlphaBlend();

#ifdef _DEBUG
	RenderDebugBundle();
	
#endif
	pGameInstance->CollisionRender();

	Render_UI();

#ifdef _DEBUG
	if (m_pRenderSetting->IsDebug())
	{
		RenderDebugGroup();

		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return;

		m_pTargetManager->Render(TEXT("MRT_Deferred"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_LightDepth"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_BlurY"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_ToonShader"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_SSAO"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Glow"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Glow_Result"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Blend"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_LUT"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_BlackWhite"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Split"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Small"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_Final"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_FinalBlend"), m_pShader, m_pVIBuffer);

		m_pTargetManager->Render(TEXT("MRT_EX"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_SSD"), m_pShader, m_pVIBuffer);
		m_pTargetManager->Render(TEXT("MRT_SSD_BLEND"), m_pShader, m_pVIBuffer);

	}
#endif

}

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTargetManager(CTargetManager::GetInstance())
	, m_pLightManager(CLightManager::GetInstance())
	, m_pRenderSetting(CRenderSetting::GetInstance())
{
	Safe_AddRef(m_pTargetManager);
	Safe_AddRef(m_pLightManager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	_uint iViewportCount = 1;
	D3D11_VIEWPORT ViewPortDesc;
	m_pContext->RSGetViewports(&iViewportCount, &ViewPortDesc);

#pragma region  RENDERTARGET_SETUP

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Diffuse", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Normal", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_OutNormal", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Depth", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Shade", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Specular", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Outline", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Outline_Result", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_ShadowMap", (_float)g_iShadowWidth, (_float)g_iShadowHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.0f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Shadow", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.0f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAO"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX"), ViewPortDesc.Width * 0.5f, ViewPortDesc.Height * 0.5f,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY"), ViewPortDesc.Width * 0.5f, ViewPortDesc.Height * 0.5f,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX_Middle"), ViewPortDesc.Width * 0.75f, ViewPortDesc.Height * 0.75f,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY_Middle"), ViewPortDesc.Width * 0.75f, ViewPortDesc.Height * 0.75f,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX_High"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY_High"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_Blend"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_Glow"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_Glow_Result"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_LUT"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlackWhite"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_Split"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_SpecGlow"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_Final"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_FinalBlend"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Static_Depth", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Diffuse_SSD", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Diffuse_Glow_SSD", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Normal_SSD", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Diffuse_SSD_Blend", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Normal_SSD_Blend", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Distortion", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_DistortionMap", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	m_pBindTargets[(_uint)TARGET::BLUR_X] = m_pTargetManager->FindTarget(L"Target_BlurX");
	m_pBindTargets[(_uint)TARGET::BLUR_Y] = m_pTargetManager->FindTarget(L"Target_BlurY");
	m_pBindTargets[(_uint)TARGET::BLUR_MIDDEL_X] = m_pTargetManager->FindTarget(L"Target_BlurX_Middle");
	m_pBindTargets[(_uint)TARGET::BLUR_MIDDEL_Y] = m_pTargetManager->FindTarget(L"Target_BlurY_Middle");
	m_pBindTargets[(_uint)TARGET::BLUR_HIGH_X] = m_pTargetManager->FindTarget(L"Target_BlurX_High");
	m_pBindTargets[(_uint)TARGET::BLUR_HIGH_Y] = m_pTargetManager->FindTarget(L"Target_BlurY_High");

	// 디퓨즈, 노말, 뎁스
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_OutNormal")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_SpecGlow")))
		return E_FAIL;

	// 그림자 깊이
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightDepth", L"Target_ShadowMap")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Shadow", L"Target_Shadow")))
		return E_FAIL;

	// 쉐이드, 스펙큘러
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;

	// 외곽선
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_ToonShader", L"Target_Outline")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Blend", L"Target_Blend")))
		return E_FAIL;

	//SSD
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SSD", L"Target_Diffuse_SSD")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SSD", L"Target_Normal_SSD")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SSD_BLEND", L"Target_Diffuse_SSD_Blend")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SSD_BLEND", L"Target_Normal_SSD_Blend")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_EX", L"Target_Static_Depth")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Glow_SSD", L"Target_Diffuse_Glow_SSD")))
		return E_FAIL;

	//Glow
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Glow", L"Target_Glow")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Glow", L"Target_DistortionMap")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Glow_Result", L"Target_Glow_Result")))
		return E_FAIL;
	//Distortion
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Distortion", L"Target_Distortion")))
		return E_FAIL;

	// SSAO
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SSAO", L"Target_SSAO")))
		return E_FAIL;

	// Black&White
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LUT", L"Target_LUT")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_BlackWhite", L"Target_BlackWhite")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Split", L"Target_Split")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SpecGlow", L"Target_SpecGlow")))
		return E_FAIL;

	// 찐막
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Final", L"Target_Final")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_FinalBlend", L"Target_FinalBlend")))
		return E_FAIL;

#pragma endregion

#pragma region POST_EFFECT_SETUP
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_DEFERRED.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader_Blur = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_BLUR.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_Blur)
		return E_FAIL;

	m_pShader_Extraction = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_EXTRACTION.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_Extraction)
		return E_FAIL;

	m_pShader_SSAO = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_SSAO.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_SSAO)
		return E_FAIL;

	m_pShader_LUT = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_LUT.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_LUT)
		return E_FAIL;

	m_pShader_RGBSplit = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_RGB_SPLIT.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_RGBSplit)
		return E_FAIL;

	m_pShader_PostEffect = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_POSTEFFECT.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_PostEffect)
		return E_FAIL;

	XMStoreFloat4x4(&m_FullScreenWorldMatrix,
		XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f) *
		XMMatrixTranslation(0.f, 0.f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

#pragma endregion

#pragma region TEXTURE_SETUP
	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/Mask/noise.jpg");

	m_pLUT[0] = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/LUT/LUT_ExposureMinus.png");
	m_pLUT[1] = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/LUT/LUT_ExposurePlus.png");
	m_pLUT[2] = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/LUT/LUT_Fuji.png");
	m_pLUT[3] = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/LUT/LUT_Grungy.png");
	m_pLUT[4] = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/LUT/LUT_South.png");
	m_pLUT[5] = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/LUT/LUT_Kuro.png");

#pragma endregion


#ifdef _DEBUG
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Diffuse"), 50.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Normal"), 50.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Shade"), 50.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Specular"), 50.f, 450.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Outline"), 50.f, 550.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_ShadowMap"), 50.f, 650.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Shadow"), 150, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_SSAO"), 150, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_LUT"), 150, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Glow"), 150, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Blend"), 150, 450.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Glow_Result"), 150, 550.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_BlackWhite"), 150, 650.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Split"), 250.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_SpecGlow"), 250.f, 150.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Diffuse_SSD"), 250, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Normal_SSD"), 250, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Diffuse_SSD_Blend"), 250, 450.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Normal_SSD_Blend"), 250, 550.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Static_Depth"), 250, 650.f, 100.f, 100.f)))
		return E_FAIL;


	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Final"), 1205.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_FinalBlend"), 1205.f, 225.f, 150.f, 150.f)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDER_GROUP eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject || eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObject[eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

void CRenderer::Render_Priority()
{
	m_pTargetManager->Begin(m_pContext, L"MRT_Deferred");

	for (auto& pGameObject : m_RenderObject[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_PRIORITY].clear();

	m_pTargetManager->End(m_pContext);
}

void CRenderer::Render_DynamicShadowMap()
{
	if (nullptr == m_pTargetManager)
		return;

	if (!m_pRenderSetting->IsActiveShadow())
	{
		for (auto& pGameObject : m_RenderObject[RENDER_DYNAMIC_SHADOW])
			Safe_Release(pGameObject);

		m_RenderObject[RENDER_DYNAMIC_SHADOW].clear();
		return;
	}

	if (FAILED(m_pTargetManager->ShadowBegin(m_pContext, L"MRT_LightDepth")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_DYNAMIC_SHADOW])
	{
		if (nullptr != pGameObject)
			pGameObject->RenderShadow();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_DYNAMIC_SHADOW].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Static()
{
	if (nullptr == m_pTargetManager)
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Deferred", VIEWPORT_TYPE::VIEWPORT_DEFAULT, false)))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_STATIC])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_STATIC].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Dynamic()
{
	if (nullptr == m_pTargetManager)
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Deferred",
		CGraphic_Device::VIEWPORT_TYPE::VIEWPORT_DEFAULT, false)))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_DYNAMIC])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_DYNAMIC].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_SSD()
{
	if (nullptr == m_pTargetManager)
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_SSD")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_SSD])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_SSD].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_SSD_BLEND()
{
	if (nullptr == m_pTargetManager)
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_SSD_BLEND")))
		return;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Diffuse", "g_DiffuseTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Normal", "g_NormalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Diffuse_SSD", "g_SSD_DiffuseTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Normal_SSD", "g_SSD_NormalTexture")))
		return;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Lights()
{
	CPipeLine* pPipeline = CPipeLine::GetInstance();
	CLightManager* pLightManager = CLightManager::GetInstance();

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_LightAcc")))
		return;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrixInv", &pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW))))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrixInv", &pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TS_PROJ))))
		return;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pPipeline->Get_CamPosition(), sizeof(_float4))))
		return;

	const LIGHT_DESC* pLightDesc = pLightManager->GetLightDesc(0);
	if (pLightDesc != nullptr)
	{
		if (FAILED(m_pShader->SetRawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
			return;
		if (FAILED(m_pShader->SetRawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
			return;
	}

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Normal_SSD_Blend", "g_NormalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Depth", "g_DepthTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Outline", "g_OutlineTexture")))
		return;
	/*if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Outline_Result", "g_OutlineTexture")))
		return;*/

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Specular", "g_SpecularTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_SpecGlow", "g_PrevGlowTexture")))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_SSAO"), "g_SSAOTexture")))
		return;

	m_pLightManager->Render(m_pShader, m_pVIBuffer);
	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Outline()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_ToonShader")))
		return;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_OutNormal"), "g_DiffuseTexture")))
		return;

	_uint iOutlinePass;
	if (m_pRenderSetting->IsActiveOutline())
		iOutlinePass = 6;
	else
		iOutlinePass = 5;

	m_pShader->Begin(iOutlinePass);
	m_pVIBuffer->Render();

	m_pTargetManager->End(m_pContext);

	//잡티제거
	//Extraction(L"Target_Outline_Result", L"Target_Outline",1, VIEWPORT_TYPE::VIEWPORT_DEFAULT);
}

void CRenderer::Render_GlowSSD()
{
	if (nullptr == m_pTargetManager)
		return;

	if (m_RenderObject[RENDER_GLOWSSD].size() == 0)
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Glow_SSD")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_GLOWSSD])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_GLOWSSD].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	CRenderTarget* pTarget = m_pTargetManager->FindTarget(L"Target_Diffuse");
	m_pTargetManager->BeginTarget(m_pContext, pTarget);

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Diffuse_SSD_Blend", "g_DiffuseTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Diffuse_Glow_SSD", "g_SSD_DiffuseTexture")))
		return;

	m_pShader->Begin(11);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	Target_Blur(L"Target_Diffuse_Glow_SSD", 1);

	CRenderTarget* pBlendTarget = m_pTargetManager->FindTarget(L"Target_Diffuse_SSD_Blend");
	m_pTargetManager->BeginTarget(m_pContext, pBlendTarget);

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Diffuse", "g_finalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Diffuse_Glow_SSD", "g_GlowOriTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY", "g_GlowTexture")))
		return;

	m_pShader_Blur->Begin(3);

	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

}

void CRenderer::Render_Particle()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Glow")))
		return;

	if (m_RenderObject[RENDER_PARTICLE].size() == 0)
		m_GlowEmpty = true;
	else
		m_GlowEmpty = false;

	for (auto& pGameObject : m_RenderObject[RENDER_PARTICLE])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_PARTICLE].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Shadow()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CLightManager* pLighrManager = CLightManager::GetInstance();
	CPipeLine* pPipeline = CPipeLine::GetInstance();

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Shadow")))
		return;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrixInv", &pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW))))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrixInv", &pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORM_STATE::TS_PROJ))))
		return;

	if (FAILED(m_pShader->SetMatrix("g_LightViewMatrix", &pLighrManager->GetLightFloat4x4(CLightManager::LIGHT_MATRIX::LIGHT_VIEW))))
		return;
	if (FAILED(m_pShader->SetMatrix("g_LightProjMatrix", &pLighrManager->GetLightFloat4x4(CLightManager::LIGHT_MATRIX::LIGHT_PROJ))))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_ShadowMap"), "g_ShadowDepthTexture")))
		return;

	m_pShader->Begin(7);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Blend()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Blend", VIEWPORT_TYPE::VIEWPORT_DEFAULT, false)))
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CLightManager* pLighrManager = CLightManager::GetInstance();
	CPipeLine* pPipeline = CPipeLine::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pPipeline->Get_CamPosition(), sizeof(_float4))))
		return;

	if (m_pRenderSetting->IsActiveBlackWhite())
	{
		_double TimeDelta = pGameInstance->GetTimeDelta();
		m_pRenderSetting->BlackWhiteTimeAcc(TimeDelta);
		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_BlackWhite"), "g_DiffuseTexture")))
			return;
	}
	else
	{
		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Diffuse_SSD_Blend"), "g_DiffuseTexture")))
			return;
	}

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_OutNormal"), "g_OutNormalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Shadow"), "g_ShadowTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Glow"), "g_GlowTexture")))
		return;

	_uint iBlendPass;

	if (m_pRenderSetting->IsActiveBlackWhite())
	{
		iBlendPass = 3;
	}
	else
	{
		if (m_pRenderSetting->IsActiveShadow())
			iBlendPass = 4;
		else
			iBlendPass = 3;
	}


	m_pShader->Begin(iBlendPass);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Glow()
{
	if (!m_GlowEmpty)
		Target_Blur(L"Target_Glow", 5);

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Glow_Result")))
		return;

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Blend", "g_finalTexture")))
		return;

	if (m_GlowEmpty)
	{
		// Blur Off
		if (m_pRenderSetting->IsActiveBlackWhite())
			m_pShader_Blur->Begin(6);
		else
			m_pShader_Blur->Begin(5);
	}
	else
	{
		// Blur On
		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Glow", "g_GlowOriTexture")))
			return;
		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY", "g_GlowTexture")))
			return;

		if (m_pRenderSetting->IsActiveBlackWhite())
			m_pShader_Blur->Begin(4);
		else
			m_pShader_Blur->Begin(3);
	}

	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Distortion()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Distortion")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_DISTORTION].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Glow_Blend()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Final")))
		return;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Glow_Result", "g_DiffuseTexture")))
		return;
	//g_NormalTexture -> DistortionTexure
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Distortion", "g_NormalTexture")))
		return;

	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_SpecularGlow()
{
	Target_Blur_Middle(L"Target_Specular", 1);

	CRenderTarget* pTarget = m_pTargetManager->FindTarget(L"Target_SpecGlow");

	m_pTargetManager->BeginTarget(m_pContext, pTarget);

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Specular", "g_finalTexture")))
		return;

	// Blur On
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Specular", "g_GlowOriTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY_Middle", "g_GlowTexture")))
		return;

	if (m_pRenderSetting->IsActiveBlackWhite())
		m_pShader_Blur->Begin(4);
	else
		m_pShader_Blur->Begin(3);

	m_pVIBuffer->Render();

	m_pTargetManager->End(m_pContext);
}

void CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObject[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_NONLIGHT].clear();
}

void CRenderer::Render_AlphaBlend()
{
	AlphaSort(RENDER_ALPHABLEND);

	for (auto& pGameObject : m_RenderObject[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_ALPHABLEND].clear();
}

void CRenderer::Render_PostEffect()
{
	CRenderSetting::FADE_DESC FadeDesc = m_pRenderSetting->GetFadeDesc();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_double TimeDelta = pGameInstance->GetTimeDelta();
	m_pRenderSetting->FadeTimeAcc(TimeDelta);

	CRenderTarget* pSourTarget = m_pTargetManager->FindTarget(L"Target_FinalBlend");
	CRenderTarget* pFianlTarget = m_pTargetManager->FindTarget(L"Target_Final");

	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pFianlTarget)))
		return;

	_float fRatio = m_pRenderSetting->GetFadeRatio();

	if (FAILED(m_pShader_PostEffect->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_PostEffect->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_PostEffect->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader_PostEffect->SetRawValue("g_fTimeAcc", &fRatio, sizeof(_float))))
		return;

	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_PostEffect, "g_FinalTexture");
	
	if(FadeDesc.m_eFadeState == CRenderSetting::FADE_IN)
		m_pShader_PostEffect->Begin(0);
	else if(FadeDesc.m_eFadeState == CRenderSetting::FADE_OUT)
		m_pShader_PostEffect->Begin(1);

	m_pVIBuffer->Render();
	
	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

}

void CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObject[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_UI].clear();
}

bool Compute(Engine::CGameObject* pSourObject, Engine::CGameObject* pDestObject)
{
	_float fSourLength = pSourObject->ComputeCameraLength();
	_float fDestLength = pDestObject->ComputeCameraLength();

	return fSourLength > fDestLength;
}

void CRenderer::AlphaSort(CRenderer::RENDER_GROUP eGroup)
{
	m_RenderObject[eGroup].sort(Compute);
}

// LUT 필터 적용
void CRenderer::ApplyLUT(_uint iIndex)
{
	CRenderTarget* pTargetDiffuse = m_pTargetManager->FindTarget(L"Target_Diffuse_SSD_Blend");
	CRenderTarget* pTargetLUT = m_pTargetManager->FindTarget(L"Target_LUT");

	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pTargetLUT)))
		return;

	if (FAILED(m_pShader_LUT->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_LUT->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_LUT->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pLUT[iIndex]->Setup_ShaderResource(m_pShader_LUT, "g_LUT")))
		return;

	if (FAILED(pTargetDiffuse->Set_ShaderResourceView(m_pShader_LUT, "g_DiffuseTexture")))
		return;

	m_pShader_LUT->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::RGBSplit(const _tchar * pBindTargetTag, const _tchar * pSourTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_float TimeDelta = (_float)pGameInstance->GetTimeDelta();
	m_pRenderSetting->RGBSpiltTimeAcc(TimeDelta);

	_float fRatio = pGameInstance->GetRGBSplitRatio();

	CRenderTarget* pSourTarget = m_pTargetManager->FindTarget(pSourTag);
	CRenderTarget* pBindTarget = m_pTargetManager->FindTarget(pBindTargetTag);

	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pBindTarget)))
		return;

	if (FAILED(m_pShader_RGBSplit->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_RGBSplit->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_RGBSplit->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader_RGBSplit->SetRawValue("g_fTimeAcc", &fRatio, sizeof(float))))
		return;

	CRenderSetting::RGB_SPLIT_DESC& Desc = pGameInstance->GetSplitDesc();
	if (FAILED(m_pShader_RGBSplit->SetRawValue("g_fDistortion", &Desc.m_fDistortion, sizeof(float))))
		return;
	if (FAILED(m_pShader_RGBSplit->SetRawValue("g_fStrength", &Desc.m_fStrength, sizeof(float))))
		return;
	if (FAILED(m_pShader_RGBSplit->SetRawValue("g_fSeparation", &Desc.m_fSeparation, sizeof(float))))
		return;

	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_RGBSplit, "g_SourTexture");

	if(m_pRenderSetting->GetSplitDir() == CRenderSetting::SPLIT_DEFAULT)
		m_pShader_RGBSplit->Begin(0);
	else
		m_pShader_RGBSplit->Begin(1);

	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

//두 텍스쳐를 더함
void CRenderer::AddCombine(const _tchar * pBindTargetTag, const _tchar * pSourTag, const _tchar * pDestTag, CGraphic_Device::VIEWPORT_TYPE eViewPortType)
{
	CRenderTarget* pBindTarget = m_pTargetManager->FindTarget(pBindTargetTag);

	CRenderTarget* pSourTarget = m_pTargetManager->FindTarget(pSourTag);
	CRenderTarget* pDestTarget = m_pTargetManager->FindTarget(pDestTag);

	switch (eViewPortType)
	{
	case VIEWPORT_TYPE::VIEWPORT_DEFAULT:
		if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pBindTarget)))
			return;
		break;
	case VIEWPORT_TYPE::VIEWPORT_SMALL:
		if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, pBindTarget)))
			return;
		break;
	default:
		break;
	}

	if (FAILED(m_pShader_Extraction->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_SourTexture");
	if (pDestTarget)
		pDestTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_DestTexture");


	m_pShader_Extraction->Begin(2);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

//두 텍스쳐를 섞음
void CRenderer::BlendCombine(const _tchar * pBindTargetTag, const _tchar * pSourTag, const _tchar * pDestTag, CGraphic_Device::VIEWPORT_TYPE eViewPortType)
{
	CRenderTarget* pBindTarget = m_pTargetManager->FindTarget(pBindTargetTag);

	CRenderTarget* pSourTarget = m_pTargetManager->FindTarget(pSourTag);
	CRenderTarget* pDestTarget = m_pTargetManager->FindTarget(pDestTag);

	switch (eViewPortType)
	{
	case VIEWPORT_TYPE::VIEWPORT_DEFAULT:
		if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pBindTarget)))
			return;
		break;
	case VIEWPORT_TYPE::VIEWPORT_SMALL:
		if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, pBindTarget)))
			return;
		break;
	default:
		break;
	}

	if (FAILED(m_pShader_Extraction->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_SourTexture");
	if (pDestTarget)
		pDestTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_DestTexture");


	m_pShader_Extraction->Begin(3);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

//두번째 인자로 들어온 렌더타겟을 바인딩된 타겟(첫번째 인자)에 렌더
void CRenderer::Extraction(const _tchar * pBindTargetTag, const _tchar * pSourTag, _uint iPass, VIEWPORT_TYPE eViewPortType)
{
	CRenderTarget* pSourTarget = m_pTargetManager->FindTarget(pSourTag);
	CRenderTarget* pBindTarget = m_pTargetManager->FindTarget(pBindTargetTag);

	switch (eViewPortType)
	{
	case VIEWPORT_TYPE::VIEWPORT_DEFAULT:
		if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pBindTarget)))
			return;
		break;
	case VIEWPORT_TYPE::VIEWPORT_SMALL:
		if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, pBindTarget)))
			return;
		break;
	default:
		break;
	}

	if (FAILED(m_pShader_Extraction->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_SourTexture");

	m_pShader_Extraction->Begin(iPass);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

// Final 렌더타겟을 백버퍼에 렌더
void CRenderer::FinalExtraction()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CRenderTarget* pSourTarget;
	CRenderTarget* pDepthTarget = m_pTargetManager->FindTarget(L"Target_Depth");

	if (m_pRenderSetting->IsFade())
	{
		Render_PostEffect();
		pSourTarget = m_pTargetManager->FindTarget(L"Target_Final");
	}
	else
	{
		pSourTarget = m_pTargetManager->FindTarget(L"Target_FinalBlend");
	}

	if (FAILED(m_pShader_Extraction->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader_Extraction->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return;

	_float2 fFog = { 100.f, 1000.f };
	if (FAILED(m_pShader_Extraction->SetRawValue("g_fFogStart", &fFog.x, sizeof(_float))))
		return;
	if (FAILED(m_pShader_Extraction->SetRawValue("g_fFogEnd", &fFog.y, sizeof(_float))))
		return;

	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_SourTexture");
	if (pDepthTarget)
		pDepthTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_DepthTexture");

	m_pShader_Extraction->Begin(5);
	m_pVIBuffer->Render();
}

void CRenderer::Target_Blur(const _tchar * TargetTag, _int BlurCount)
{
	if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_X])))
		return;

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	_float2 Resolution = { 640, 360 };
	if (FAILED(m_pShader_Blur->SetRawValue("g_Resolution", &Resolution, sizeof(_float2))))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, TargetTag, "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_Y])))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX", "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(1);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	for (int i = 1; BlurCount > i; i++)
	{
		if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_X])))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(0);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;

		if (FAILED(m_pTargetManager->SmallBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_Y])))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(1);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;
	}
}

void CRenderer::Target_Blur_Middle(const _tchar * TargetTag, _int BlurCount)
{
	if (FAILED(m_pTargetManager->MiddleBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_MIDDEL_X])))
		return;

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	_float2 Resolution = { 960, 540 };
	if (FAILED(m_pShader_Blur->SetRawValue("g_Resolution", &Resolution, sizeof(_float2))))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, TargetTag, "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	if (FAILED(m_pTargetManager->MiddleBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_MIDDEL_Y])))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX_Middle", "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(1);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	for (int i = 1; BlurCount > i; i++)
	{
		if (FAILED(m_pTargetManager->MiddleBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_MIDDEL_X])))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY_Middle", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(0);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;

		if (FAILED(m_pTargetManager->MiddleBeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_MIDDEL_Y])))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX_Middle", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(1);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;
	}
}

void CRenderer::Target_Blur_High(const _tchar * TargetTag, _int BlurCount)
{
	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_HIGH_X])))
		return;

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	_float2 Resolution = { 1280.f, 720.f };
	if (FAILED(m_pShader_Blur->SetRawValue("g_Resolution", &Resolution, sizeof(_float2))))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, TargetTag, "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_HIGH_Y])))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX_High", "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(1);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	for (int i = 1; BlurCount > i; i++)
	{
		if (FAILED(m_pTargetManager->BeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_HIGH_X])))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY_High", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(0);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;

		if (FAILED(m_pTargetManager->BeginTarget(m_pContext, m_pBindTargets[(_uint)TARGET::BLUR_HIGH_Y])))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX_High", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(1);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;
	}
}

void CRenderer::Ready_SSAO(const _tchar* pBindTargetTag)
{
	CRenderTarget* pBindTarget = m_pTargetManager->FindTarget(pBindTargetTag);

	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pBindTarget)))
		return;

	if (FAILED(m_pShader_SSAO->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_SSAO->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_SSAO->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_SSAO, L"Target_Diffuse_SSD_Blend", "g_DiffuseTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_SSAO, L"Target_Normal_SSD_Blend", "g_NormalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_SSAO, L"Target_Depth", "g_DepthTexture")))
		return;

	if (FAILED(m_pShader_SSAO->Begin(0)))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CRenderer";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();
	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_Extraction);
	Safe_Release(m_pShader_SSAO);
	Safe_Release(m_pShader_LUT);
	Safe_Release(m_pShader_RGBSplit);
	Safe_Release(m_pShader_PostEffect);

	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pLightManager);
	Safe_Release(m_pTargetManager);


	if (m_isClone)
		return;

	for (int i = 0; i < LUT_DEFAULT; ++i)
		Safe_Release(m_pLUT[i]);

#ifdef _DEBUG
	for (auto& pDebugBundle : m_DebugRenderBundle)
	{
		if (nullptr != pDebugBundle)
			Safe_Release(pDebugBundle);
	}
	m_DebugRenderBundle.clear();
#endif // _DEBUG

}

#ifdef _DEBUG
HRESULT CRenderer::AddDebugGroup(CComponent * pComponent)
{
	m_DebugRenderObjects.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::RenderDebugGroup()
{
	for (auto& pComponent : m_DebugRenderObjects)
	{
		pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugRenderObjects.clear();

	return S_OK;
}
HRESULT CRenderer::AddDebugBundle(CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (false == m_DebugBundle_Render)
		return S_OK;

	m_DebugRenderBundle.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::RenderDebugBundle()
{
	if (false == m_DebugBundle_Render)
		return S_OK;

	for (auto& pComponent : m_DebugRenderBundle)
	{
		pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugRenderBundle.clear();

	return S_OK;
}
#endif
