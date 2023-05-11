#include "stdafx.h"
#include "..\Public\Application.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GUIManager.h"
#include "AnimToolManager.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Intro.h"
#include "BackGround.h"
#include "DynamicCamera.h"
#include "IntroCamera.h"

#include "Floor.h"
#include "Character.h"



CApplication::CApplication()
	: m_pGameInstance { CGameInstance::GetInstance() }
#ifdef _DEBUG
	, m_pGUIManager(CGUIManager::GetInstance())
	, m_pAnimToolManager {CAnimToolManager::GetInstance()}
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

	m_pRenderer->DebugBundleRender_Control(true);

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
	m_pGameInstance->RenderLevelUI();
	m_pGUIManager->Render();

	m_pContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);
#endif

	m_pGameInstance->Clear_RenderTargetView(_float4(0.f, 0.f, 0.f, 0.f));
	m_pGameInstance->Clear_DepthStencilView();

	m_pRenderer->Draw();
	m_pGameInstance->CollisionRender();

#ifdef _DEBUG
	m_pRenderer->RenderDebugBundle();
#endif // _DEBUG

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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUIFFER_SSD_BOX,
		CVIBuffer_SSDBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUIFFER_RECT_NOR,
		CVIBuffer_Rect_Nor::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::UI,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::UI_SUB,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_UI_SUB.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::VTXNORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXNORTEX.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::POINT_INST,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE_DECLARATION::Elements, VTXPOINTINSTANCE_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::SKILL_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/Shader_SkillModel.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, SHADER::SKILL_RECT,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/Shader_VTXSKILLTEX.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UILOADING,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Loading/loading%d.dds"), 11))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Grass_003.jpg")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::FLOOR,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Floor.dds")))))
		return E_FAIL;

#pragma region TERRAIN
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUFFER_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Height3.bmp"), TEXT("../../Data/GamePlay/Terrain/Height_Map/Vertices.data")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Gra_01_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Roc_01A_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_06_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_14_D.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Gra_01_N.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Roc_01A_N.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_06_N.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_14_N.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_FILTER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Terrain/Filter_Map/Filter.dds")))))
		return E_FAIL;
	
#pragma endregion TERRAIN

#pragma region CHR_SELECT_TERRAIN
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUFFER_CHARACTERSELECT_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Loby_Height.bmp"), TEXT("../../Data/Lobby/Terrain/Height_Map/Vertices.data")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_5,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_08_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_6,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_10a_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_7,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_10_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_D_8,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_09b_D.dds")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_5,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_08_N.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_6,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_10_N.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_7,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_10_N.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::TERRAIN_N_8,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_09_N.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::CHARACTERSELECT_TERRAIN_FILTER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Data/Lobby/Terrain/Filter_Map/Filter.dds")))))
		return E_FAIL;
#pragma endregion CHR_SELECT_TERRAIN

	CVIBuffer_Terrain::TERRAIN_SIZE tSize;
	tSize.mX = 300;
	tSize.mZ = 300;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUFFER_FLOOR_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, tSize))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::BACKGROUND_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/background_mask.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::BLACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/black.jpg")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::BACKGROUND_DEFAULT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/background_default.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::BACKGROUND_INTRO,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/background_start.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::LOADING_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/LoadingText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::LOADING_BACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/LoadingBack.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::LOADING_FRONT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/LoadingFront.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::LOADING_SIDE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/SideFont.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::SPHERE,
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::OBB,
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::CARCULATOR,
		CCalculator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// TEXT("../../Data/GamePlay/Navigation/Navigation.data")
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::NAVIGATION,
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Navigation/Navigation.data")))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CApplication::Ready_Prototype_Static_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::FLOOR, CFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::BACKGROUND, CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::INTRO, CIntro::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::DYNAMIC_CAMERA,
		CDynamicCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::INTRO_CAMERA,
		CIntroCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Ready_Static_Effect()))
		return E_FAIL;
	


	return S_OK;
}

HRESULT CApplication::InitializeManager()
{
	CAppManager::GetInstance();
	CGameMode::GetInstance();

#ifdef _DEBUG
	m_pAnimToolManager->Init_AnimToolManager(m_pDevice, m_pContext);
#endif

	return S_OK;
}

void CApplication::DestroyManager()
{
#ifdef _DEBUG
	m_pGUIManager->Shutdown();

	m_pAnimToolManager = nullptr;
	CAnimToolManager::DestroyInstance();
#endif

	CAppManager::DestroyInstance();
	CGameMode::DestroyInstance();
}

HRESULT CApplication::Ready_Static_Effect()
{
	CGameMode* pGameModeInst = CGameMode::GetInstance();
	for (_int i = 0; 20 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/YangYang_Jump_Attack_01.bin"),
			"../../Resource/Effect/YangYang_Jump_Attack_01/")))
		{
			MSG_BOX("Create Failed : YangYang_Jump_Attack_01");
			return E_FAIL;
		}
			
	}

	for (_int i = 0; 20 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/Test_SSD.bin"),
			"../../Resource/Effect/Test_SSD/")))
		{
			MSG_BOX("Create Failed : Test_SSD");
			return E_FAIL;
		}
	}

	for (_int i = 0; 20 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/Test_GLOW_SSD.bin"),
			"../../Resource/Effect/Test_GLOW_SSD/")))
		{
			MSG_BOX("Create Failed : Test_GLOW_SSD");
			return E_FAIL;
		}
	}

	for (_int i = 0; 2 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/Loby_Particle_Effect_01.bin"),
			"../../Resource/Effect/Loby_Particle_Effect/")))
		{
			MSG_BOX("Create Failed : Loby_Particle_Effect_01");
			return E_FAIL;
		};
	}

	for (_int i = 0; 20 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice , m_pContext , g_hWnd, TEXT("../../Resource/Effect/Loby_Particle_Effect_02.bin"),
			"../../Resource/Effect/Loby_Particle_Effect/")))
		{
			MSG_BOX("Create Failed : Loby_Particle_Effect_02");
			return E_FAIL;
		}
	}

	for (_int i = 0; 30 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/Double_Jump_B.bin"),
			"../../Resource/Effect/Double_Jump/")))
		{
			MSG_BOX("Create Failed : Double_Jump_B");
			return E_FAIL;
		}
	}

	for (_int i = 0; 30 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/Double_Jump_F.bin"),
			"../../Resource/Effect/Double_Jump/")))
		{
			MSG_BOX("Create Failed : Double_Jump_F");
			return E_FAIL;
		}
	}
	for (_int i = 0; 10 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, TEXT("../../Resource/Effect/Test_Loby_Effect.bin"),
			"../../Resource/Effect/Test_Loby_Effect/")))
		{
			MSG_BOX("Create Failed : Test_Loby_Effect");
			return E_FAIL;
		}
	}

	
	return S_OK;
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
