#include "stdafx.h"
#include "..\Public\Application.h"

#include "AppManager.h"
#include "GUIManager.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "BackGround.h"
#include "DynamicCamera.h"

#include "Character.h"

CApplication::CApplication()
	: m_pGameInstance { CGameInstance::GetInstance() }
#ifdef _DEBUG
	, m_pGUIManager(CGUIManager::GetInstance())
#endif
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CApplication::Initialize()
{
	GRAPHIC_DESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GRAPHIC_DESC);
	GraphicDesc.eMode = GRAPHIC_DESC::MODE_WINDOW;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInstance = g_hInst;
	
	if (FAILED(m_pGameInstance->Engine_Initialize(GraphicDesc, LEVEL_END, COLL_END, &m_pDevice, &m_pContext)))
		return E_FAIL;

#ifdef _DEBUG
	m_pContext->OMGetRenderTargets(1, &m_pRTV, &m_pDSV);
	m_pRTV->Release();
	m_pDSV->Release();
	m_pGUIManager->Initialize(m_pDevice, m_pContext, g_hWnd);
#endif

	if (FAILED(InitializeManager()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Static_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Static_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	//콜라이더 보이기/숨기기
	m_pGameInstance->SetCollisionDebugRender(true);

	return S_OK;
}

void CApplication::Tick(_double TimeDelta)
{
	iTickCount++;
	srand((unsigned int)time(nullptr) + iTickCount);

	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Engine_Tick(TimeDelta);
}

HRESULT CApplication::Render()
{
	if (!m_pGameInstance || !m_pRenderer)
		return E_FAIL;

#ifdef _DEBUG
	m_pGUIManager->NewFrame();
	m_pGameInstance->RenderGUI();
	m_pGUIManager->Render();
	m_pContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);
#endif

	m_pGameInstance->Clear_RenderTargetView(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencilView();
	m_pRenderer->Draw();
	m_pGameInstance->CollisionRender();

#ifdef _DEBUG
	m_pGUIManager->RenderDrawData();
#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CApplication::Open_Level(LEVEL_ID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT hr = 0;
	wstring message;
	switch (eLevelID)
	{
	case LEVEL_STATIC:
	case LEVEL_LOADING:
		message = L"CApplication::Open_Level : index check";
		MESSAGE(message);
		break;
	default:
		hr = m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID));
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::Ready_Prototype_Static_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::RENDERER,
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::TRANSFORM,
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::VTXTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXTEX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::GROUD,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_GROUD.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::PHONG,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_PHONG.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::VTXNORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXNORTEX.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Tile0.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUFFER_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::BACKGROUND,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/background.jpg")))))
		return E_FAIL;

#pragma region COLLIDER
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::SPHERE,
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::OBB,
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CApplication::Ready_Prototype_Static_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::DYNAMIC_CAMERA,
		CDynamicCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::CHARACTER,
		CCharacter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::InitializeManager()
{
	CAppManager::GetInstance();

	return S_OK;
}

void CApplication::DestroyManager()
{
#ifdef _DEBUG
	m_pGUIManager->Shutdown();
#endif

	CAppManager::DestroyInstance();
}

CApplication* CApplication::Create()
{
	CApplication* pInstance = new CApplication();

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CApplication";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CApplication::Free()
{
	DestroyManager();

	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	CGameInstance::Engine_Release();
}
