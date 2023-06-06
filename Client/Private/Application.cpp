#include "stdafx.h"
#include "..\Public\Application.h"

#include "ItemDB.h"
#include "GameMode.h"
#include "AppManager.h"
#include "GUIManager.h"
#include "AnimToolManager.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Intro.h"
#include "BackGround.h"

#include "PlayerState.h"
#include "CameraMovement.h"
#include "DynamicCamera.h"
#include "IntroCamera.h"
#include "ActionCam.h"

#include "Floor.h"
#include "Character.h"

// Item
#include "Item.h"

//UI
#include "AcquireUI.h"
#include "InteractionUI.h"
#include "AcquireSystem.h"
#include "TerminalUI.h"
#include "ResonatorUI.h"

CApplication::CApplication()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pItemDB { CItemDB::GetInstance() }
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

	if (FAILED(Ready_Fonts()))
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

	//UI에 필요한 텍스쳐 원본 생성A

	//콜라이더 보이기/숨기기
	m_pGameInstance->SetCollisionDebugRender(true);
#ifdef _DEBUG
	m_pRenderer->DebugBundleRender_Control(false);
#endif

	return S_OK;
}

void CApplication::Tick(_double TimeDelta)
{
	iTickCount++;
	m_TimeAcc += TimeDelta;
	//srand((unsigned int)time(nullptr) + iTickCount);

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

	/* Render가 한번 불릴때마다 카운트 증가 */
	++m_iNumRender;

	/* 1초마다 그리는 회수를 측정 */
	if (1.0 <= m_TimeAcc)
	{
		wsprintf(m_szFPS, TEXT("Fps : %d"), m_iNumRender);
		m_TimeAcc = 0.0;
		m_iNumRender = 0;
	}

	m_pGameInstance->Render_Font(
		TEXT("DefaultFont"),
		m_szFPS,
		_float2(0.f, 0.f),
		XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float2(0.5f, 0.5f)
	);
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

	CGameMode* pGM = CGameMode::GetInstance();
	pGM->SetupRenderer(m_pRenderer);

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
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::VIBUFFER_TERRAIN,
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
		return E_FAIL;*/
	
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

#pragma region ROCK_DIFFUSE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::ROCK_D_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockDiffuse/T4_Com_Roc_03A_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::ROCK_D_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockDiffuse/T4_Com_Roc_03B_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::ROCK_D_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockDiffuse/T4_Com_Roc_03C_D.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::ROCK_D_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockDiffuse/T_Roc_20_D.dds")))))
		return E_FAIL;
#pragma endregion ROCK_DIFFUSE

#pragma region GRASS_MASK
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::GRASS_MASK_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/GrassMask/T_Mask_001.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::GRASS_MASK_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/GrassMask/T_Mask_20006.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::GRASS_MASK_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/GrassMask/T_Mask_20022.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::GRASS_MASK_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/GrassMask/T_Mask_30093_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::GRASS_MASK_5,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/GrassMask/T_RingDissolve_30001.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::GRASS_MASK_6,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/GrassMask/T_Waterfall_Directional_Noise.png")))))
		return E_FAIL;

#pragma endregion GRASS_MASK

#pragma region ROCK_MASK
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_SUB_D,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockDiffuse/T4_Gra_01_D.dds")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_SUB_D");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_5,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_05A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_5");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_6,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_06A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_6");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_7,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_07A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_7");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_8,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_08A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_8");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_9,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_09A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_9");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_10,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_10A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_10");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_11,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_11A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_11");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_12,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_12A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_12");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_14,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_14A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_14");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_15,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_15A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_15");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_16,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_16A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_16");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_17,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_17A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_17");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_22,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_22A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_22");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_23,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_23A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_23");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_24,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_24A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_24");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_25,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_25A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_25");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_26,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_26A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_26");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_27,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_27A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_27");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_28,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_28A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_28");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_29,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_29A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_29");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_32,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_32A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_32");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_35,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_35A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_35");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_41,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_41A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_41_A");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_41_B,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_41B_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_41_B");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_42,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_42A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_42");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_43,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_43A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_43");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_44,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_44A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_44");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_45,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_45A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_45");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_50,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_50A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_50");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_52,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_52A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_52");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_55,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_55A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_55");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_66,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_66A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_66");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_67,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_67A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_67");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_68,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_68A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_68");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_70,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_70A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_70");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_71,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_71A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_71");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_72,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_72A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_72");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_76,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_76A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_76");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_77,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_77A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_77");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_78,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_78A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_78");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_80,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_80A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_80");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_82,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_82A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_82");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ROCK_MASK_83,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/RockMask/T_Com_Roc_83A_M.png")))))
	{
		MSG_BOX("Failed to Add Texture in Application : ROCK_MASK_83");
		return E_FAIL;
	}
	
#pragma endregion ROCK_MASK
	
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

	// 각 레벨마다 네비게이션 생성해서 사용하도록 수정
	// TEXT("../../Data/GamePlay/Navigation/Navigation.data")
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, COMPONENT::NAVIGATION,
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Navigation/Navigation.data")))))
	//	return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CApplication::Ready_Prototype_Static_GameObject()
{
	if (FAILED(Ready_UI_Data()))
		return E_FAIL;

	if (FAILED(Ready_Item_Image()))
		return E_FAIL;

	if (FAILED(Ready_Item_Data()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::FLOOR, CFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::BACKGROUND, CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::INTRO, CIntro::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::STATIC_CAM_MOVEMENT,
		CCameraMovement::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::DYNAMIC_CAMERA,
		CDynamicCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::INTRO_CAMERA,
		CIntroCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::ACTION_CAM_BANGSUN,
		CActionCam::Create(m_pDevice, m_pContext, CCameraMovement::CAM_BANGSUN))))	
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::STATIC_CHARACTER_STATE,
		CPlayerState::Create(m_pDevice, m_pContext))))
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
	CAnimToolManager::DestroyInstance();
	m_pAnimToolManager = nullptr;
#endif

	CItemDB::DestroyInstance();
	CAppManager::DestroyInstance();
	CGameMode::DestroyInstance();
}

HRESULT CApplication::Ready_UI_Data()
{
	//Mouse
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_MOUSE_BTN_LEFT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/MouseLeft_Btn.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_MOUSE_BTN_MIDDLE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/MouseMiddel_Btn.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_MOUSE_BTN_RIGHT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/MouseRight_Btn.dds")))))
		return E_FAIL;

	//Key
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_KEY_BTN_F,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/KeyF_Btn.dds")))))
		return E_FAIL;
	// Etc
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_LISTBACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/GradBack.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_LISTGARD_SPRITE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/ListGardSprite4x4.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_LISTLIGHT_SPRITE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/ListLightSprite.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ICON_ACTIVATE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/icon_activate.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ICON_INSPECT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/icon_inspect.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ICON_CHEST,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/icon_chest.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ACTIVATE_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/activate_text.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_INSPECT_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/insptect_text.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_SIMPLE_CHEST_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/simple_chest_text.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_STANDARD_CHEST_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/standard_chest_text.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_EXPANDED_CHEST_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/expanded_chest_text.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::ACQUIRE_SYSTEM,
		CAcquireSystem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::UI_INTERACTION,
		CInteractionUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::UI_ACQUIRE,
		CAcquireUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::UI_TERMINAL,
		CTerminalUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::UI_TERMINAL_RESONATOR,
		CResonatorUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::Ready_Item_Image()
{

#pragma region ITEM_ICON
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_TACTREITE_VOUCHER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconCommon/T_IconA160_zcpq_UI.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_COMMEMORATIVE_COIN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconCommon/T_IconA160_currency_5_UI.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_TACTITE_COIN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconCommon/T_IconA160_hsb_UI.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_CASKET,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconCommon/T_IconMst160_006_UI.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_CASKETPIECE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconCommon/T_IconMst_001_1_UI.png")))))
		return E_FAIL;

#pragma endregion

#pragma region TEXT
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/3.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/4.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_5,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/5.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_6,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/6.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_7,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/7.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_8,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/8.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_9,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/9.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_X,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Number/x.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_ACQUIRE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Text/Acquire.dds")))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TACTREITE_VOUCHER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/Text/TacetreiteVoucher.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_COMMEMORATIVE_COIN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/Text/CommemorativeCoin.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TACTITE_COIN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/Text/TacetiteCoin.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_CASKET,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/Text/Casket.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_CASKETPIECE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/Text/CasketPiece.dds")))))
		return E_FAIL;

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::IMAGE_LISTBACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/GradBack.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::IMAGE_GLOWGARD,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Background/GlowGardBack.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::IMAGE_SIDEALPHA,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Background/SideAlphaBack.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::IMAGE_GLOWLINE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Bar/GlowLine.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::MASK_TWINKL,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Mask/Twinkl.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::MASK_DESSOLVE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Mask/DessolveMask.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERNINAL_BACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEFT0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal5.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEFT1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEFTHALF_IN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEFTHALF_OUT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal3.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_PLAYER_IMAGE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal4.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEVEL_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/LevelText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEVELSUB_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/LevelSubText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_LEVELEXP_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/LevelExpText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::IMAGE_WHITE_RECT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/WhiteRect.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ID,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IDBlur.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_SLOT_NONE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal8.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_SLOT_CLICK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal9.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_RANK_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/RankText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_RANKSUB_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/RankSubText.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_RESONATORS,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/ResonatorsText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_BACKPACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/BackpackText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_TERMINAL,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/ResonanceTerminalText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_MISSION,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/MissionsText.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_TEAM,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/TeamText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_MERGE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/MergeText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_MAP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/MapText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_TEXT_SETTING,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/SettingText.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_RESONATORS,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconResonator.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_BACKPACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconBackpack.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_TERMINAL,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconTerminal.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_MISSION,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconMission.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_TEAM,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconTeam.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_MERGE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconMerge.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_MAP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconMap.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_TERMINAL_ICON_SETTING,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/IconSetting.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_DETAILPANEL,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/DetailPanel.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_GRADBACKGROUND,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/GradBack.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_GLOWBUTTON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/GlowButton.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_GLOW_SOLID_BUTTON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/GlowSolidButton.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_RESONATOR_STATETEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StatsDetailTitle.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_RESONATOR_WEAPONTEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/WeaponTitle.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_RESONATOR_ECHOTEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/EchoTitle.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_RESONATOR_RESONANCETEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/ResonanceTitle.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_RESONATOR_WUTHERIDETEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/WutherideTitle.dds")))))
		return E_FAIL;


	// Resonator UI
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_RESONATOR_STATE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StateIcon.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_RESONATOR_WEAPON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/WeaponIcon.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_RESONATOR_ECHO,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/EchoIcon.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_RESONATOR_RENONANCE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/ResonanceIcon.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_RESONATOR_WUTHERIDE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/WutherideIcon.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::RESONATOR_STATE_HP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StateHP.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::RESONATOR_STATE_ATTACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StateAttack.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::RESONATOR_STATE_DEFENSE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StateDefense.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::RESONATOR_STATE_CRITRATE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StateCritRate.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_RESONATOR_ROVER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Rover.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_RESONATOR_YANGYANG,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Yangyang.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_RESONATOR_CHIXIA,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Chixia.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_RESONATOR_LEVEL,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/StateLevelText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_SLASH,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/TextSlash.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_PER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Per.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_DOT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Dot.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::GLOW_CIRCLE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/GlowCircle.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_ROVER_GRAD,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/RoverGrad.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_YANGYANG_GRAD,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/YangyangGrad.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GUNNER_GRAD,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/GunnerGrad.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_EXP0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/StandardCapsule.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_EXP1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/ExtandedCapsule.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_EXP2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/HighendCapsule.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_EXP3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/UniqueCapsule.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_EXP0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/IconExp0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_EXP1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/IconExp1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_EXP2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/IconExp2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_EXP3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Items/IconRub/IconExp3.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::SLOT_EMPTY,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/SlotEmpty.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::SLOT_UPGRADE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/UpgradeSlot.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::SPRITE_UI_FLASH,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/SpriteFlash6x5.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_UPGRADE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/UpgradeText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_GLOW_UPARROW_BUTTON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/GlowUpArrowButton.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_CONSUME,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/ConsumeText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_OWN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/OwnText.dds")))))
		return E_FAIL;

	// 무기 타이틀
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_SWORD_0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleSword0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_SWORD_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleSword1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_SWORD_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleSword2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_SWORD_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleSword3.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_SWORD_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleSword4.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_GUN_0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleGun0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_GUN_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleGun1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_GUN_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleGun2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_GUN_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleGun3.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_TITLE_GUN_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextTitleGun4.dds")))))
		return E_FAIL;

	// 무기 아이콘
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_SWORD_0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconSword0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_SWORD_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconSword1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_SWORD_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconSword2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_SWORD_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconSword3.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_SWORD_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconSword4.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GUN_0,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconGun0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GUN_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconGun1.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GUN_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconGun2.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GUN_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconGun3.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GUN_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconGun4.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_UPGRADE_SIGN,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/UpgradeSign.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_ATTACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/AttackText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_WEAPONDESC,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/TextWeaponDesc.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::IMAGE_WHITE_GRADBACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/WhiteGradBack.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_CRITICAL_DAMAGE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/CDText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_PHISICAL_RATE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/PDText.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_SWITCH,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/SwitchText.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_WEAPON_UPGRADE_BUTTON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/wu.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::UI_WEAPON_SWITCH_BUTTON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/ws.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_UPGRADECOST,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/UpgradeCost.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ICON_GEM,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/IconGem.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::TEXT_GEM,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/StaticImage/GemText.dds")))))
		return E_FAIL;

	//이미지

	return S_OK;
}

HRESULT CApplication::Ready_Item_Data()
{

	return S_OK;
}

HRESULT CApplication::Ready_Static_Effect()
{
	CGameMode* pGameModeInst = CGameMode::GetInstance();

	//Lobby_Effect
	for (_int i = 0; 2 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::LOBBY,
			TEXT("../../Resource/Effect/Player_Nvzhu/Lobby_Particle_Effect_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Lobby_Particle_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::LOBBY,
			TEXT("../../Resource/Effect/Player_Nvzhu/Lobby_Particle_Effect_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Lobby_Particle_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::LOBBY,
			TEXT("../../Resource/Effect/Test_Lobby_Effect.bin"),
			"../../Resource/Effect/Test_Lobby_Effect/")))
			return E_FAIL;
	}

#pragma region Player_Nvzhu

	for (_int i = 0; 3 > i; i++)
	{
		
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Double_Jump_B.bin"),
			"../../Resource/Effect/Player_Nvzhu/Double_Jump/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Double_Jump_F.bin"),
			"../../Resource/Effect/Player_Nvzhu/Double_Jump/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack_03.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack_04.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack_05_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack_05_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Link_Effect.bin"),
			"../../Resource/Effect/Player_Nvzhu/Link_Effect/")))
			return E_FAIL;
		
		//돌진공격 터지는 이펙트 위치는 플레이어 World_Matrix
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Attack_Po_2_Boom_Effect.bin"),
			"../../Resource/Effect/Player_Nvzhu/Attack_Po/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Attack_Po_2_Effect.bin"),
			"../../Resource/Effect/Player_Nvzhu/Attack_Po/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Attack_Po_2_Smoke.bin"),
			"../../Resource/Effect/Player_Nvzhu/Attack_Po/")))
			return E_FAIL;
		// 돌진후 돌아오면서 터트리는 이펙트(위의 돌진공격이 명중 시 추가 연계 후속타 느낌(?))
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Attack_Po_3_Effect.bin"),
			"../../Resource/Effect/Player_Nvzhu/Attack_Po/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Skill_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Skill/")))
			return E_FAIL;
		
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Skill_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Skill/")))
			return E_FAIL;

		//sKill_02 쓸떄 나오는 원반
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Skill_02_Circle.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Skill/")))
			return E_FAIL;
		
		//칼 집어 넣을때 나오는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Sheath_Effect.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Sheath_Effect/")))
			return E_FAIL;

		//필살기

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Burst_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Burst/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Burst_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Burst/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Burst_03.bin"),
			"../../Resource/Effect/Player_Nvzhu/Burst/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Burst_04.bin"),
			"../../Resource/Effect/Player_Nvzhu/Burst/")))
			return E_FAIL;

		// 회피 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Comon/P_Evasion_Effect_N.bin"),
			"../../Resource/Effect/Comon/P_Evasion_Effect/")))
			return E_FAIL;

		for (_int j = 0; 2 > j; j++)
		{
			// 공중에서 바닥에 내리찍는 공격
			if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
				TEXT("../../Resource/Effect/Player_Nvzhu/Rib_AirAttack.bin"),
				"../../Resource/Effect/Player_Nvzhu/Rib_AirAttack/")))
				return E_FAIL;
		}
	}

	//Nvzhu 기본 공격에 따른 히트 이펙트
	for (_int i = 0; 30 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		// Nvzhu 투사체들 공용 피격 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_03.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_04.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_05_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_05_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		// Skill_01 , SKill_02  공용 피격 이펙트 
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_R.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;
	}

#pragma endregion

#pragma region Player_Chixia

	for (_int i = 0; 3 > i; i++)
	{
		// 강공격 중공격 약공격 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Shoot_B.bin"),
			"../../Resource/Effect/Player_Chixia/Shoot/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Shoot_M.bin"),
			"../../Resource/Effect/Player_Chixia/Shoot/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Shoot_S.bin"),
			"../../Resource/Effect/Player_Chixia/Shoot/")))
			return E_FAIL;
		// 강공격 히트 이펙트 , 일반 히트 이펙트 , 일반 히트 크리티컬 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Hit_Effect_B.bin"),
			"../../Resource/Effect/Player_Chixia/Hit/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Hit_Effect_S.bin"),
			"../../Resource/Effect/Player_Chixia/Hit/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Hit_Effect_SC.bin"),
			"../../Resource/Effect/Player_Chixia/Hit/")))
			return E_FAIL;

		//차지 샷 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Charge_Shoot.bin"),
			"../../Resource/Effect/Player_Chixia/Charge/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Chixia_Charge_Hit.bin"),
			"../../Resource/Effect/Player_Chixia/Charge/")))
			return E_FAIL;

		//기본 총알
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Bullet.bin"),
			"../../Resource/Effect/Player_Chixia/Bullet/")))
			return E_FAIL;

		// 연사 스킬 총알
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Skill_Bullet_S.bin"),
			"../../Resource/Effect/Player_Chixia/Bullet/")))
			return E_FAIL;

		// 강공 스킬 총알
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_CHIXIA,
			TEXT("../../Resource/Effect/Player_Chixia/Skill_Bullet_B.bin"),
			"../../Resource/Effect/Player_Chixia/Bullet/")))
			return E_FAIL;

		// 회피 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Comon/P_Evasion_Effect_C.bin"),
			"../../Resource/Effect/Comon/P_Evasion_Effect/")))
			return E_FAIL;

	}
#pragma endregion

#pragma region Yangyang
	for (_int i = 0; 3 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Attack_01.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_YangYang_Attack_02.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_YangYang_Attack_03.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_YangYang_Attack_04_1.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_YangYang_Attack_04_2.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Attack_05.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Attack_06.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;

		// 공중 공격 후 착지할때 나오는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_AirAttack_End_1.bin"),
			"../../Resource/Effect/Player_Yangyang/AirAttack/")))
			return E_FAIL;
		// 착지 후 검집 닫을떄 한번 더 폭발하는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_AirAttack_End_2.bin"),
			"../../Resource/Effect/Player_Yangyang/AirAttack/")))
			return E_FAIL;
		
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_AirAttack_Start_2_1.bin"),
			"../../Resource/Effect/Player_Yangyang/AirAttack/")))
			return E_FAIL;

		// Start2_1 이 끝나갈때쯤  내려찍기 직전에 기모이는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_AirAttack_Start_2_2.bin"),
			"../../Resource/Effect/Player_Yangyang/AirAttack/")))
			return E_FAIL;

		//Burst 회오리 : 미사일은 터레인 높이 태우면 좋을것 같음.
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Burst_01.bin"),
			"../../Resource/Effect/Player_Yangyang/Burst/")))
			return E_FAIL;
		//Burst 회오리 끝나고 터지는 이펙트 블러 처리 해줘야함. 높이 높여줄 필요 없음. Burst_01 높이 그대로
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Burst_02.bin"),
			"../../Resource/Effect/Player_Yangyang/Burst/")))
			return E_FAIL;

		//Skill_01
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Skill_01_01.bin"),
			"../../Resource/Effect/Player_Yangyang/Burst/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Skill_01_02.bin"),
			"../../Resource/Effect/Player_Yangyang/Burst/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/P_Yangyang_Skill_02.bin"),
			"../../Resource/Effect/Player_Yangyang/Attack/")))
			return E_FAIL;
		
		// 회피 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Comon/P_Evasion_Effect_Y.bin"),
			"../../Resource/Effect/Comon/P_Evasion_Effect/")))
			return E_FAIL;

	}
	//Yangyang 기본 공격에 따른 히트 이펙트
	for (_int i = 0; 30 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_01.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_02.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_3.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_4_1.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_4_2.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_5.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_YANGYANG,
			TEXT("../../Resource/Effect/Player_Yangyang/Hit_Effect_Y_6.bin"),
			"../../Resource/Effect/Player_Yangyang/Hit_Effect/")))
			return E_FAIL;
	}
#pragma endregion


#pragma region Comon

	for (_int i = 0; 10 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Flare_Effect.bin"),
			"../../Resource/Effect/Comon/Flare/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_01_Black.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_01_Gray.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_02_Black.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_02_Gray.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_03_Black.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_03_Gray.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_04_Black.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_04_Gray.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Smoke_05_Gray.bin"),
			"../../Resource/Effect/Comon/Smoke/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Monster_Parry_Distortion.bin"),
			"../../Resource/Effect/Comon/Parry_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Monster_Parry_Effect.bin"),
			"../../Resource/Effect/Comon/Parry_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Parry_Effect.bin"),
			"../../Resource/Effect/Comon/Parry_Effect/")))
			return E_FAIL;


		// 아이템 획득 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Get_Item_Effect_01.bin"),
			"../../Resource/Effect/Comon/Item/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::COMON,
			TEXT("../../Resource/Effect/Comon/Get_Item_Effect_02.bin"),
			"../../Resource/Effect/Comon/Item/")))
			return E_FAIL;

	}
#pragma endregion

#pragma region Tutorial_Monster

	for (_int i = 0; 5 > i; i++)
	{
		//Monster_Gazizi
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL ,
			TEXT("../../Resource/Effect/M_GAzizi/GenkiDama_Boom.bin"),
			"../../Resource/Effect/M_GAzizi/GenkiDama/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_GAzizi/GenkiDama_Ready.bin"),
			"../../Resource/Effect/M_GAzizi/GenkiDama/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_GAzizi/GenkiDama_Shoot.bin"),
			"../../Resource/Effect/M_GAzizi/GenkiDama/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_GAzizi/GenkiDama_Ready_02.bin"),
			"../../Resource/Effect/M_GAzizi/GenkiDama/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_GAzizi/GenkiDama_Shoot_02.bin"),
			"../../Resource/Effect/M_GAzizi/GenkiDama/")))
			return E_FAIL;

		//Monster_Zigzag
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Zigzag/M_Boom.bin"),
			"../../Resource/Effect/M_Zigzag/Boom/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Zigzag/M_Rush_Ready.bin"),
			"../../Resource/Effect/M_Zigzag/Rush/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Zigzag/M_Rush_Start.bin"),
			"../../Resource/Effect/M_Zigzag/Rush/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Zigzag/Awukaka_Jump.bin"),
			"../../Resource/Effect/M_Zigzag/Rush/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Zigzag/Awukaka_Attack_02.bin"),
			"../../Resource/Effect/M_Zigzag/Rush/")))
			return E_FAIL;


		// 낫든 몬스터 등장 이팩트

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/Comon/M_Appear_Effect.bin"),
			"../../Resource/Effect/Comon/M_Appear_Effect/")))
			return E_FAIL;

		// 낫든애 공격 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Anjin_Attack_01.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Anjin_Attack_02_1.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Anjin_Attack_02_2.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Anjin_Attack_02_3.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Anjin_Attack_03.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;

		//Mark 는 공격전  빛나면서 플레이어에게 공격 타이밍 알려주는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Attack_Mark.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;
		//Hit 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Anjin/Anjin_Hit.bin"),
			"../../Resource/Effect/M_Anjin/Anjin_Attack/")))
			return E_FAIL;

		//활쏘는 몬스터 Leilie
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Leilie/M_Leilie_Arrow.bin"),
			"../../Resource/Effect/M_Leilie/Arrow/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Leilie/M_Leilie_Arrow_Charge.bin"),
			"../../Resource/Effect/M_Leilie/Arrow/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Leilie/M_Leilie_Arrow_Shoot.bin"),
			"../../Resource/Effect/M_Leilie/Arrow/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Leilie/M_Leilie_Enforce_Arrow.bin"),
			"../../Resource/Effect/M_Leilie/Arrow/")))
			return E_FAIL;

		//총쏘는 정령 초록이 FHuxiuxiu

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_FHuxiuxiu/FHuxiuxiu_Attack_01.bin"),
			"../../Resource/Effect/M_FHuxiuxiu/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_FHuxiuxiu/FHUxiuxiu_Attack_02.bin"),
			"../../Resource/Effect/M_FHuxiuxiu/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_FHuxiuxiu/FHUxiuxiu_Attack_Auror.bin"),
			"../../Resource/Effect/M_FHuxiuxiu/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_FHuxiuxiu/FHUxiuxiu_Bullet_01.bin"),
			"../../Resource/Effect/M_FHuxiuxiu/Attack/")))
			return E_FAIL;

		// 신사 질럿 M_Huojin

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Huojin/Huojin_Attack_01_1.bin"),
			"../../Resource/Effect/M_Huojin/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Huojin/Huojin_Attack_01_3.bin"),
			"../../Resource/Effect/M_Huojin/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Huojin/Huojin_Attack_02.bin"),
			"../../Resource/Effect/M_Huojin/Attack/")))
			return E_FAIL;

		// 팽그리 (부메랑)

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Fenglie/M_Fenglie_Weapon.bin"),
			"../../Resource/Effect/M_Fenglie/Weapon/")))
			return E_FAIL;

		// 빙그리 (얼음창)

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Binglie/M_Binglie_Weapon.bin"),
			"../../Resource/Effect/M_Binglie/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Binglie/M_Binglie_Shoot_Ready.bin"),
			"../../Resource/Effect/M_Binglie/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Binglie/M_Binglie_Ball.bin"),
			"../../Resource/Effect/M_Binglie/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Binglie/M_Binglie_Ball_Ready.bin"),
			"../../Resource/Effect/M_Binglie/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Binglie/M_Binglie_Ball_Boom.bin"),
			"../../Resource/Effect/M_Binglie/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Binglie/M_Binglie_Shoot_Start.bin"),
			"../../Resource/Effect/M_Binglie/Attack/")))
			return E_FAIL;
		
		// 스킬쓰면서 지속적으로 빛나는 무기 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Fenglie/M_Fenglie_Weapon.bin"),
			"../../Resource/Effect/M_Fenglie/Weapon/")))
			return E_FAIL;

		// 번개 내리꽂히는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::TUTORIAL,
			TEXT("../../Resource/Effect/M_Fenglie/M_LeiFa_Lightning.bin"),
			"../../Resource/Effect/M_Fenglie/Lightning/")))
			return E_FAIL;

	}
	
#pragma endregion

	//Boss_Crownless
	for (_int i = 0; 3 > i; i++)
	{
		// 모션들 중 빠르게 이동하는 모션 시전 시 나오는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_Dash.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

#pragma region Page_01
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_02_1.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_02_2.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_03_1.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_03_2.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_03_Flare.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_04_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_04_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_04_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_04_Flare.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_05_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_05_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		// 총알
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_05_Bullet.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		// Attack_8 이펙트들
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_08.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_08_Circle.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P1_Attack_08_Jump.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;
#pragma endregion

#pragma region Page_02
		//Page_02
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_03_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_03_Dash.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_03_Flare.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_05_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_05_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_05_02_Boom.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_05_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_05_Flare.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_06_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_06_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_06_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_07_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_07_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_10_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_10_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_17_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_17_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_17_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_17_04.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_18_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_19_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_19_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_19_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_19_04.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_20_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_20_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_20_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_20_04.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P2_Attack_20_05.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;
#pragma endregion

#pragma region Page_03

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_01_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_01_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_01_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_07_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_07_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_07_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;
		
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_09_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;
		
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_10_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_10_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_10_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_10_04.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_10_05.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_10_06.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_11_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_11_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_11_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_01.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_02.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_03.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_04.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_05.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_05_1.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_05_2.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;
		
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_06.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::BOSS_CROWNLESS,
			TEXT("../../Resource/Effect/B_Crownless/B_Crownless_P3_Attack_13_07.bin"),
			"../../Resource/Effect/B_Crownless/Attack/")))
			return E_FAIL;
		
#pragma endregion

	}

	return S_OK;
}

HRESULT CApplication::Ready_Fonts()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Font_MapleStoryBold */
	if (FAILED(m_pGameInstance->Add_Font(
		m_pDevice,
		m_pContext,
		TEXT("DefaultFont"),
		TEXT("../../Resource/Fonts/HeirofLightBold.spritefont")
	)))
		return E_FAIL;

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
