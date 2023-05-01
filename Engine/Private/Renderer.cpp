#include "..\Public\Renderer.h"
#include "GameInstance.h"
#include "TargetManager.h"
#include "LightManager.h"
#include "PipeLine.h"

#include "GameObject.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

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

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_StaticShadowDepth", (_float)g_iShadowWidth, (_float)g_iShadowHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.0f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, L"Target_DynamicShadowDepth", (_float)g_iShadowWidth, (_float)g_iShadowHeight,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.0f, 1.f))))
		return E_FAIL;


#pragma region Blur

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY"), ViewPortDesc.Width, ViewPortDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.0f))))
		return E_FAIL;

#pragma endregion

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
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_StaticShadow", L"Target_StaticShadowDepth")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightDepth", L"Target_DynamicShadowDepth")))
		return E_FAIL;

	// ½¦ÀÌµå, ½ºÆåÅ§·¯
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;

	// ¿Ü°û¼±
	if (FAILED(m_pTargetManager->AddMRT(L"MRT_ToonShader", L"Target_Outline")))
		return E_FAIL;

#pragma region MRT_Blur

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_BlurX", L"Target_BlurX")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddMRT(L"MRT_BlurY", L"Target_BlurY")))
		return E_FAIL;

#pragma endregion


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_DEFERRED.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader_Blur = CShader::Create(m_pDevice, m_pContext, L"../../Shader/SHADER_BLUR.hlsl", VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	if (nullptr == m_pShader_Blur)
		return E_FAIL;

	XMStoreFloat4x4(&m_FullScreenWorldMatrix,
		XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f) *
		XMMatrixTranslation(0.f, 0.f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

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
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_StaticShadowDepth"), 1100, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_Debug(TEXT("Target_DynamicShadowDepth"), 350, 350.f, 300.f, 300.f)))
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

void CRenderer::Draw()
{
	Render_Priority();

	if(!m_bBakeShadowed)
		Render_StaticShadowDepth();

	Render_DynamicShadowDepth();
	Render_NonAlphaBlend();
	Render_Lights();
	Render_Outline();
	Render_Blend();
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
	m_pTargetManager->Render(TEXT("MRT_StaticShadow"), m_pShader, m_pVIBuffer);
	m_pTargetManager->Render(TEXT("MRT_LightDepth"), m_pShader, m_pVIBuffer);
	m_pTargetManager->Render(TEXT("MRT_ToonShader"), m_pShader, m_pVIBuffer);
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

void CRenderer::Render_StaticShadowDepth()
{
	if (nullptr == m_pTargetManager)
		return;

	m_bBakeShadowed = true;

	if (FAILED(m_pTargetManager->StaticShadowBegin(m_pContext, L"MRT_StaticShadow")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_STATIC_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->DrawStaticShadow();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_STATIC_SHADOWDEPTH].clear();

	if (FAILED(m_pTargetManager->End(m_pContext)))
		return;
}

void CRenderer::Render_DynamicShadowDepth()
{
	if (nullptr == m_pTargetManager)
		return;

	if (FAILED(m_pTargetManager->ShadowBegin(m_pContext, L"MRT_LightDepth")))
		return;

	for (auto& pGameObject : m_RenderObject[RENDER_DYNAMIC_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->DrawDynamicShadow();

		Safe_Release(pGameObject);
	}

	m_RenderObject[RENDER_DYNAMIC_SHADOWDEPTH].clear();

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

void CRenderer::Render_Blend()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CLightManager* pLighrManager = CLightManager::GetInstance();
	CPipeLine* pPipeline = CPipeLine::GetInstance();

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
	if (FAILED(m_pShader->SetMatrix("g_BakeLightViewMatrix", &pLighrManager->GetBakeLightFloat4x4(CLightManager::LIGHT_MATRIX::LIGHT_VIEW))))
		return;
	if (FAILED(m_pShader->SetMatrix("g_LightProjMatrix", &pLighrManager->GetLightFloat4x4(CLightManager::LIGHT_MATRIX::LIGHT_PROJ))))
		return;

	if (FAILED(m_pShader->SetRawValue("g_vLightPos", &pGameInstance->GetLightPosition(), sizeof(_float4))))
		return;

	const LIGHT_DESC* LightDesc = pGameInstance->GetLightDesc(0);
	if (LightDesc)
	{
		if (FAILED(m_pShader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
			return;
	}

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

 	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_StaticShadowDepth"), "g_StaticShadowDepthTexture")))
		return;

	if (FAILED(m_pTargetManager->Set_ShaderResourceView(m_pShader, TEXT("Target_DynamicShadowDepth"), "g_ShadowDepthTexture")))
		return;

	m_pShader->Begin(3);
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
