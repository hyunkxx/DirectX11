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

// Item
#include "Item.h"

//UI
#include "InteractionUI.h"
#include "AcquireSystem.h"

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

	//UI에 필요한 텍스쳐 원본 생성

	m_pGameInstance->SetCollisionDebugRender(true);
#ifdef _DEBUG
	//콜라이더 보이기/숨기기
	m_pRenderer->DebugBundleRender_Control(false);
#endif

	return S_OK;
}

void CApplication::Tick(_double TimeDelta)
{
	iTickCount++;
	srand((unsigned int)time(nullptr) + iTickCount);

	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Engine_Tick(TimeDelta);

#ifdef _DEBUG
	m_TimeAcc += TimeDelta;
#endif // _DEBUG

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

#pragma region FPS
#ifdef _DEBUG
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
		TEXT("HeirofLightBold"),
		m_szFPS,
		_float2(0.f, 0.f),
		XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float2(0.5f, 0.5f)
	);
#endif // _DEBUG

#ifdef _DEBUG
	m_pRenderer->RenderDebugBundle();
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

HRESULT CApplication::Ready_UI_Data()
{
	//Mouse
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_MOUSE_BTN_LEFT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/MouseLeft_Btn.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_MOUSE_BTN_MIDDLE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/MouseMiddel_Btn.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_MOUSE_BTN_RIGHT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/MouseRight_Btn.png")))))
		return E_FAIL;

	//Key
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_KEY_BTN_F,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/KeyF_Btn.png")))))
		return E_FAIL;
	// Etc
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_LISTBACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/ListBack.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_LISTGARD_SPRITE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/ListGardSprite.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_LISTLIGHT_SPRITE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/ListLightSprite.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ICON_ACTIVATE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/icon_activate.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ICON_INSPECT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/icon_inspect.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ICON_CHEST,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/icon_chest.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_ACTIVATE_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/activate_text.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_INSPECT_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/insptect_text.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_SIMPLE_CHEST_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/simple_chest_text.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_STANDARD_CHEST_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/standard_chest_text.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXTURE::UI_EXPANDED_CHEST_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/UI/Button/expanded_chest_text.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::ACQUIRE_SYSTEM,
		CAcquireSystem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::UI_INTERACTION,
		CInteractionUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::Ready_Item_Image()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, STATIC_IMAGE::ITEM_DEFAULT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Items/IconCook80/T_IconCook80_001_UI.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Texture(STATIC_IMAGE::ITEM_DEFAULT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Items/IconCook80/T_IconCook80_001_UI.png")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::Ready_Item_Data()
{
	if (FAILED(m_pGameInstance->Add_Prototype(OBJECT::ITEM,
		CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::Ready_Static_Effect()
{
	CGameMode* pGameModeInst = CGameMode::GetInstance();
	
#pragma region Player_Nvzhu

	//Lobby_Effect
	for (_int i = 0; 2 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU ,
			TEXT("../../Resource/Effect/Player_Nvzhu/Loby_Particle_Effect_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Loby_Particle_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Loby_Particle_Effect_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Loby_Particle_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU ,
			TEXT("../../Resource/Effect/Test_Loby_Effect.bin"),
			"../../Resource/Effect/Test_Loby_Effect/")))
			return E_FAIL;
	}

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
		// 공중에서 바닥에 내리찍는 공격
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Rib_AirAttack.bin"),
			"../../Resource/Effect/Player_Nvzhu/Rib_AirAttack/")))
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
		
		//칼 집어 넣을때 나오는 이펙트
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Nvzhu_Sheath_Effect.bin"),
			"../../Resource/Effect/Player_Nvzhu/Nvzhu_Sheath_Effect/")))
			return E_FAIL;
		
	}

	//Nvzhu 기본 공격에 따른 히트 이펙트
	for (_int i = 0; 5 > i; i++)
	{
		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU, 
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_01.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

		if (FAILED(pGameModeInst->Add_Effect(m_pDevice, m_pContext, g_hWnd, EFFECT_ID::PLAYER_NVZHU,
			TEXT("../../Resource/Effect/Player_Nvzhu/Hit_Effect_02.bin"),
			"../../Resource/Effect/Player_Nvzhu/Hit_Effect/")))
			return E_FAIL;

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
	}

#pragma endregion

#pragma region Comon

	for (_int i = 0; 5 > i; i++)
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
	}
#pragma endregion


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
		TEXT("HeirofLightBold"),
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
