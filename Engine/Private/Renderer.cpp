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

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTargetManager(CTargetManager::GetInstance())
	, m_pLightManager(CLightManager::GetInstance())
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

#pragma region  AddRenderTargets
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

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_ShadowMap", (_float)g_iShadowWidth, (_float)g_iShadowHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.0f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_Shadow", ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.0f, 1.f))))
		return E_FAIL;


	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAO"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;


	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
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


	// µðÇ»Áî, ³ë¸», µª½º
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Deferred", L"Target_OutNormal")))
		return E_FAIL;

	// ±×¸²ÀÚ ±íÀÌ
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightDepth", L"Target_ShadowMap")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Shadow", L"Target_Shadow")))
		return E_FAIL;

	// ½¦ÀÌµå, ½ºÆåÅ§·¯
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;

	// ¿Ü°û¼±
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_ToonShader", L"Target_Outline")))
		return E_FAIL;
	
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Blend", L"Target_Blend")))
		return E_FAIL;

	// ºí·¯
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_BlurX", L"Target_BlurX")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_BlurY", L"Target_BlurY")))
		return E_FAIL;

	//Glow
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Glow", L"Target_Glow")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_Glow_Result", L"Target_Glow_Result")))
		return E_FAIL;

	// SSAO
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_SSAO", L"Target_SSAO")))
		return E_FAIL;
#pragma endregion

#pragma region POST_EFFECT
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

	XMStoreFloat4x4(&m_FullScreenWorldMatrix,
		XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f) *
		XMMatrixTranslation(0.f, 0.f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

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

	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Glow"), 150, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Blend"), 150, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_Glow_Result"), 150, 450.f, 100.f, 100.f)))
		return E_FAIL;

#endif

	m_pNoiseTexture = CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Texture/Mask/noise.jpg");

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

void CRenderer::Draw()
{
	Render_Priority();
	Render_DynamicShadowMap();
	Render_NonAlphaBlend();

	Ready_SSAO(L"Target_SSAO");
	Target_Blur(L"Target_SSAO", 2);
	Extraction(L"Target_SSAO", L"Target_BlurY");

	Render_Lights();
	Render_Outline();
	Render_Shadow();

	Target_Blur(L"Target_Shadow", 2);
	Extraction(L"Target_Shadow", L"Target_BlurY");

	Render_Particle();

	Render_Blend();
	
	Render_Glow();
	Render_Glow_Blend();

	Render_NonLight();
	Render_AlphaBlend();
	Render_UI();

#ifdef _DEBUG
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
#endif

}

void CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObject[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	
	m_RenderObject[RENDER_PRIORITY].clear();
}

void CRenderer::Render_DynamicShadowMap()
{
	if (nullptr == m_pTargetManager)
		return;

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

void CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTargetManager)
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Deferred")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_NONALPHA].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Lights()
{
	CPipeLine* pPipeline = CPipeLine::GetInstance();

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

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Normal", "g_NormalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Depth", "g_DepthTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Outline", "g_OutlineTexture")))
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

	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Particle()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Glow")))
		return;

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

	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Blend()
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_Blend")))
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

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return;
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

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_Glow()
{
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

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_Glow", "g_GlowOriTexture")))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY", "g_GlowTexture")))
		return;

	m_pShader_Blur->Begin(3);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

}

void CRenderer::Render_Glow_Blend()
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, L"Target_Glow_Result", "g_DiffuseTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();
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

//µÎ¹øÂ° ÀÎÀÚ·Î µé¾î¿Â ·»´õÅ¸°ÙÀ» ¹ÙÀÎµùµÈ Å¸°Ù(Ã¹¹øÂ° ÀÎÀÚ)¿¡ ·»´õ
void CRenderer::Extraction(const _tchar * pBindTargetTag, const _tchar * pSourTag)
{
	CRenderTarget* pSourTarget = m_pTargetManager->FindTarget(pSourTag);
	CRenderTarget* pBindTarget = m_pTargetManager->FindTarget(pBindTargetTag);

	if (FAILED(m_pTargetManager->BeginTarget(m_pContext, pBindTarget)))
		return;

	if (FAILED(m_pShader_Extraction->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Extraction->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;
	
	if (pSourTarget)
		pSourTarget->Set_ShaderResourceView(m_pShader_Extraction, "g_SourTexture");

	m_pShader_Extraction->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Target_Blur(const _tchar * TargetTag, _int BlurCount)
{
	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_BlurX")))
		return;

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, TargetTag, "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_BlurY")))
		return;

	if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX", "g_BlurTexture")))
		return;

	m_pShader_Blur->Begin(1);
	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;

	for (int i = 1; BlurCount > i; i++)
	{
		if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_BlurX")))
			return;

		if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
			return;
		if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return;
		if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurY", "g_BlurTexture")))
			return;

		m_pShader_Blur->Begin(0);
		m_pVIBuffer->Render();

		if (FAILED(m_pTargetManager->End(m_pContext)))
			return;

		if (FAILED(m_pTargetManager->Begin(m_pContext, L"MRT_BlurY")))
			return;

		if (FAILED(m_pShader_Blur->SetMatrix("g_WorldMatrix", &m_FullScreenWorldMatrix)))
			return;
		if (FAILED(m_pShader_Blur->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return;
		if (FAILED(m_pShader_Blur->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return;

		if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_Blur, L"Target_BlurX", "g_BlurTexture")))
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

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_SSAO, L"Target_Diffuse", "g_DiffuseTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_SSAO, L"Target_Normal", "g_NormalTexture")))
		return;
	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader_SSAO, L"Target_Depth", "g_DepthTexture")))
		return;

	m_pNoiseTexture->Setup_ShaderResource(m_pShader_SSAO, "g_NoiseTexture");

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

	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pVIBuffer);
	
	Safe_Release(m_pLightManager);
	Safe_Release(m_pTargetManager);

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
#endif
