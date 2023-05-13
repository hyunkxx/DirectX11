#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "FloatingStone.h"
#include "Terrain.h"
#include "CharacterSelect_Terrain.h"
#include "Floor.h"

#include "Lobby_Rock.h"

#include "Lobby_Character.h"
#include "PlayerGirl.h"
#include "Parts.h"

#include "PlayerCamera.h"

#include "Tree_0.h"
#include "Tree_1.h"

#include "Sky.h"

#include "UI_MainScreen.h"
#include "UI_Mouse.h"
#include "UI_Minimap.h"
#include "UI_Terminal.h"
#include "UI_TapT.h"
#include "UI_Monster.h"

//AnimTool
#include "AnimToolManager.h"
#include "TestVTF.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pApp = Client::CAppManager::GetInstance();
}

unsigned int APIENTRY ThreadEntry(void* pArg)
{
	CLoader* pLoader = ((CLoader*)pArg);
	EnterCriticalSection(pLoader->Get_CriticalSectionPtr());

	switch (pLoader->Get_NextLevel())
	{
	case LEVEL_LOGO:
		pLoader->Load_Level_Logo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Load_Level_GamePlay();
		break;
	case LEVEL_ANIMTOOL:
		pLoader->Load_Level_AnimTool();
		break;
	}

	LeaveCriticalSection(pLoader->Get_CriticalSectionPtr());
	return 0;
}

HRESULT CLoader::Initialize(LEVEL_ID eNextLevel)
{
	CoInitializeEx(nullptr, 0);
	 
	m_eNextLevel = eNextLevel;
	InitializeCriticalSection(&m_hCriticalSection);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Level_Logo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Logo Component
#pragma region COMPONENTS
	
	m_szLoadingStateText = L"텍스쳐를 로딩중입니다.";
	m_pApp->LoadRatio(0.1f);
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::EYE_BURST,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeBurst.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::EYE_MASK, 
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeMask.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::TAPSTART_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/TapStartText.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::LOGO_APLLY,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/ApplyText.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::LOGO_TEXT_BACKGROUND,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/TextBack.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::CHOOSE_ROVER_BACK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/ChooseRoverBack.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::CHOOSE_ROVER_TEXT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/ChooseRoverText.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::TWINKL_ON_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Mask/TwinklOn.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::TWINKL_OFF_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Mask/TwinklOFF.png")))))
		return E_FAIL;

	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";
	m_pApp->LoadRatio(0.2f);

	m_szLoadingStateText = L"모델를 로딩중입니다.";
	m_pApp->LoadRatio(0.5f);
	// SMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_FLOATING_ROCK_01, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/FloatingRock_01.smdl")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_FLOATING_ROCK_01_LOD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/FloatingRock_01_LOD.smdl")))))
		return E_FAIL;


	// DMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_LOBBY_CHARACTER_MODEL, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/LobbyPlayerGirl.dmdl")))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_LOBBY_MALE_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/LobbyMale/LobbyMale_Model.dmdl")))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_LOBBY_MALE_ANIMSET, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/LobbyMale/LobbyMale_Anim.dmdl")))))
	//	return E_FAIL;

#pragma region LOBBY_ROCK
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/0/SM_Com_Roc_28AS.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/1/SM_Com_Roc_52AS.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_2, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/2/SM_Com_Roc_76AL.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_3, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/3/SM_Com_Roc_77AL.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_4, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/4/SM_Com_Roc_78AL.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_5, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/5/SM_Com_Roc_82AM.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_LOBBY_ROCK_6, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/6/SM_Com_Roc_83AM.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader Logo : SMD_LOBBY_ROCK_6");
		return E_FAIL;
	}
#pragma endregion LOBBY_ROCK

	m_szLoadingStateText = L"셰이더를 로딩중입니다.";
	m_pApp->LoadRatio(0.7f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SKY_LOBBY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/Sky_Lobby.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SKY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/SM_SkysphereFlat2.smdl")))))
		return E_FAIL;

#pragma endregion

	//Logo GameObject
#pragma region GAMEOBJECTS
	
	m_szLoadingStateText = L"객체 원형을 준비중";
	m_pApp->LoadRatio(0.8f);
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_CHARACTER_LEFT, CLobbyCharacter::Create(m_pDevice, m_pContext, DMODEL::DMD_PLAYERGIRL_MODEL, DMODEL::DMD_LOBBY_CHARACTER_MODEL, 0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_CHARACTER_RIGHT, CLobbyCharacter::Create(m_pDevice, m_pContext, DMODEL::DMD_PLAYERGIRL_MODEL, DMODEL::DMD_LOBBY_CHARACTER_MODEL, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::SKY, CSky::Create(m_pDevice, m_pContext, CSky::DEFAULT))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::SKY_LOBBY, CSky::Create(m_pDevice, m_pContext, CSky::LOBBY))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::FLOATING_STONE, CFloatingStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHARACTERSELECT_TERRAIN, CCharacterSelect_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region LOBBY_ROCK
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_ROCK, CLobby_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion LOBBY_ROCK

#pragma endregion

	m_szLoadingStateText = L"Load Completed";
	m_pApp->LoadRatio(1.f);

	m_isFinish = true;
	return S_OK;
}

HRESULT CLoader::Load_Level_GamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//GamePlay Component
#pragma region COMPONENTS
	m_szLoadingStateText = L"텍스쳐를 로딩중입니다.";
	// UI
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UI,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Main/main%d.dds"), 112))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMOUSE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Mouse/CursorPre%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UITERMINAL,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Terminal/Terminal%d.dds"), 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMAP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/Map.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMAPDEFAULT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/DefaultIcon%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMAPICON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/Icon%d.dds"), 43))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIFIGHT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Fight/Fight%d.dds"), 33))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::EYE_BURST, 
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeBurst.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::EYE_MASK, 
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeMask.dds"))))
		return E_FAIL;

	m_pApp->LoadRatio(0.1f);
	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";

	m_pApp->LoadRatio(0.2f);
	m_szLoadingStateText = L"모델를 로딩중입니다.";

	// SMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
		return E_FAIL;

	// DMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;
	m_pApp->LoadRatio(0.6f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_TREE_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/0/SM_Com_Tre_05AH.smdl"), TEXT("../../Data/GamePlay/MapObject/Tree_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : Tree_0");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_TREE_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/1/SM_Com_Tre_02AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Tree_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : Tree_1");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SKY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/SM_SkysphereFlat2.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SKY_LOBBY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/Sky_Lobby.smdl")))))
		return E_FAIL;

	m_pApp->LoadRatio(0.9f);
	m_szLoadingStateText = L"셰이더를 로딩중입니다.";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SHADER::MODEL_INSTANCE, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL_INSTANCE.hlsl"), VTXSMODELINSTANCE_DECLARATION::Elements, VTXSMODELINSTANCE_DECLARATION::ElementCount))))
	{
		MSG_BOX("Failed to Prototype In Loader : MODEL_INSTANCE_SHADER");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	m_pApp->LoadRatio(1.f);
	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Terrain/UVSamplerRatio.data")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYER_CAMERA, CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYERGIRL, CPlayerGirl::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SWORD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SWORD))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SCABBARD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SCABBARD))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_0, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_1, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::OBJECT_TREE_0, CTree_0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::OBJECT_TREE_1, CTree_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//UIUI
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::UI, CUI_MainScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MOUSE, CUI_Mouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::UIMINIMAP, CUI_Minimap::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::UITAPT, CUI_TapT::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::UIMONSTER, CUI_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::UITERMINAL, CUI_Terminal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	m_Start = (_uint)time(nullptr);

	while (true)
	{
		m_Wait = (_uint)time(nullptr);

		if (m_Wait - m_Start >= 3)
			break;
	}

	m_szLoadingStateText = L"Load Completed";
	m_isFinish = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_AnimTool()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CAnimToolManager* pAnimTool = CAnimToolManager::GetInstance();

	//GamePlay Component
#pragma region COMPONENTS
	m_szLoadingStateText = L"텍스쳐를 로딩중입니다.";

	m_pApp->LoadRatio(0.1f);
	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";

	m_pApp->LoadRatio(0.3f);
	m_szLoadingStateText = L"모델를 로딩중입니다.";

	// SModel
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
		return E_FAIL;

	// DModel
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;
	m_pApp->LoadRatio(0.5f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	m_pApp->LoadRatio(0.6f);
	m_szLoadingStateText = L"셰이더를 로딩중입니다.";
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

#pragma endregion

	m_pApp->LoadRatio(1.f);
	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Terrain/UVSamplerRatio.data")))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_NULL, CTestGeneric::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	CTestVTF::TESTVTF_DESC tDesc;
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_PLAYERGIRL_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_SWORD_0_SWORD;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_SWORD_0_SCABBARD;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_PLAYERGIRL, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_PLAYERGIRL), TEXT("PlayerGirl"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SWORD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SWORD))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SCABBARD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SCABBARD))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_0, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_1, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_1))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(OBJECT::FLOOR, CFloor::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

#pragma endregion

	m_szLoadingStateText = L"Load Completed";
	m_isFinish = true;
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL_ID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		wstring message = L"Failed to Create : CLoader";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	DeleteCriticalSection(&m_hCriticalSection);
	CloseHandle(m_hThread);
}
