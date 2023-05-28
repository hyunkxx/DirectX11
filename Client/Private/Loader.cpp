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
#include "P_PlayerGirl.h"
#include "Parts.h"

#include "Sandbag.h"
#include "M_GAzizi.h"
#include "M_Anjin.h"
#include "M_AWukaka.h"

#include "PlayerState.h"
#include "Inventory.h"
#include "InteractionObject.h"
#include "Chest.h"

#include "PlayerCamera.h"

#include "MapObject.h"

#include "Sky.h"

#include "UI_MainScreen.h"
#include "UI_Mouse.h"
#include "UI_Minimap.h"
#include "UI_TapT.h"
#include "UI_Monster.h"
#include "UI_Tip.h"

#include "Missile.h"
#include "Missile_Constant.h"
#include "Missile_RotAround.h"

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
	case LEVEL_TEST:
		pLoader->Load_Level_Test();
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
	
	m_szLoadingStateText = L"�ؽ��ĸ� �ε����Դϴ�.";
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Background/ChooseRoverText.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::TWINKL_ON_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Mask/TwinklOn.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXTURE::TWINKL_OFF_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Mask/TwinklOFF.png")))))
		return E_FAIL;

	m_szLoadingStateText = L"�������۸� �ε����Դϴ�.";
	m_pApp->LoadRatio(0.2f);

	m_szLoadingStateText = L"�𵨸� �ε����Դϴ�.";
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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_FLOATING_ROCK_01, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/FloatingRock_01.smdl")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_FLOATING_ROCK_01_LOD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Object/Rocks/Lobby_Rock/FloatingRock_01_LOD.smdl")))))
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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_LOBBY_MALE_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/LobbyMale/Male_Model.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, DMODEL::DMD_LOBBY_MALE_ANIMSET, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/LobbyMale/Male_Anim.dmdl")))))
		return E_FAIL;

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

	m_szLoadingStateText = L"���̴��� �ε����Դϴ�.";
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
	
	m_szLoadingStateText = L"��ü ������ �غ���";
	m_pApp->LoadRatio(0.8f);
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::INVENTORY, CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_CHARACTER_LEFT, CLobbyCharacter::Create(m_pDevice, m_pContext, DMODEL::DMD_PLAYERGIRL_MODEL, DMODEL::DMD_LOBBY_CHARACTER_MODEL, 0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_CHARACTER_RIGHT, CLobbyCharacter::Create(m_pDevice, m_pContext, DMODEL::DMD_LOBBY_MALE_MODEL, DMODEL::DMD_LOBBY_MALE_ANIMSET, 1))))
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
	m_szLoadingStateText = L"�ؽ��ĸ� �ε����Դϴ�.";
	// UI
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UI,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Main/main%d.dds"), 255))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMOUSE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Mouse/CursorPre%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMAP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/Map%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMAPDEFAULT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/DefaultIcon%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIMAPICON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/Icon%d.dds"), 47))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UIFIGHT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Fight/Fight%d.dds"), 79))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::UITIP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Tip/Tip%d.dds"), 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::EYE_BURST, 
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeBurst.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::EYE_MASK, 
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeMask.dds"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::BOX_EMISSIVE,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Static/Prop/Interaction/SimpleBox/box_emissive.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::BOX_SPECULAR,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Static/Prop/Interaction/SimpleBox/box_spec.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::BOX_SSAO,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Static/Prop/Interaction/SimpleBox/box_ssao.dds"))))
		return E_FAIL;

	m_pApp->LoadRatio(0.1f);
	m_szLoadingStateText = L"�������۸� �ε����Դϴ�.";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, COMPONENT::NAVIGATION,
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Navigation/Navigation.data")))))
		return E_FAIL;


	m_pApp->LoadRatio(0.2f);
	m_szLoadingStateText = L"�𵨸� �ε����Դϴ�.";

	// SMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SIMPLE_BOX, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Interaction/SimpleBox/SimpleBox.smdl")))))
		return E_FAIL;

	// DMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;
	m_pApp->LoadRatio(0.6f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_MONSTER_CROWNLESS_P1, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Boss/Crownless/Crownless_P3.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_MONSTER_GAZIZI, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Spirit/GAzizi.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_MONSTER_ANJIN, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Anjin.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_MONSTER_AWUKAKA, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Spirit/AWukaka.dmdl")))))
		return E_FAIL;

#pragma region TREE
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/0/SM_Com_Tre_02AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/1/SM_Com_Tre_03AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/2/SM_Com_Tre_04AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/3/SM_Com_Tre_05AH.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/4/SM_Com_Tre_07AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/5/SM_Com_Tre_07BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/6/SM_Com_Tre_07CS.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/7/SM_Com_Tre_09AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/8/SM_Com_Tre_10AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/9/SM_Com_Tre_11AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_9");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_10, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/10/SM_Com_Tre_12AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_10.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_10");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_11, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/11/SM_Com_Tre_13AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_11.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_11");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_12, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/12/SM_Com_Tre_14AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_12.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_12");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_13, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/13/SM_Com_Tre_15AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_13.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_13");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_14, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/14/SM_Com_Tre_15BL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_14.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_14");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_15, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/15/SM_Com_Tre_16AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_15.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_15");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_16, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/16/SM_Com_Tre_16BL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_16.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_16");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_17, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/17/SM_Com_Tre_17AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_17.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_17");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_18, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/18/SM_Com_Tre_18AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_18.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_18");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_19, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/19/SM_Com_Tre_19AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_19.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_19");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_20, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/20/SM_Com_Tre_21AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_20.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_20");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_21, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/21/SM_Com_Tre_27BL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_21.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_21");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_22, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/22/SM_Com_Tre_28AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_22.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_22");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_23, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/23/SM_Com_Tre_29BM.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_23.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_23");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_24, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/24/SM_Com_Tre_30AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_24.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_24");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_25, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/25/SM_Com_Tre_32BH.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_25.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_25");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_26, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/26/SM_Com_Tre_33AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_26.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_26");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_27, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/27/SM_Com_Tre_34AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_27.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_27");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_28, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/28/SM_Com_Tre_34BL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_28.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_28");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_29, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/29/SM_Com_Tre_36AH.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_29.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_29");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_TREE_30, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/30/SM_Com_Tre_38AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Trees/Tree_30.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_TREE_30");
		return E_FAIL;
	}
#pragma endregion TREE

#pragma region ROCK
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/0/SM_Com_Roc_05AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/1/SM_Com_Roc_06AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/2/SM_Com_Roc_07AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/3/SM_Com_Roc_08AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/4/SM_Com_Roc_09AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/5/SM_Com_Roc_10AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/6/SM_Com_Roc_11AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/7/SM_Com_Roc_12AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/8/SM_Com_Roc_13AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/9/SM_Com_Roc_14AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_9");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_10, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/10/SM_Com_Roc_15AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_10.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_10");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_11, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/11/SM_Com_Roc_16AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_11.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_11");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_12, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/12/SM_Com_Roc_17AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_12.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_12");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_13, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/13/SM_Com_Roc_22AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_13.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_13");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_14, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/14/SM_Com_Roc_23AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_14.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_14");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_15, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/15/SM_Com_Roc_24AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_15.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_15");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_16, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/16/SM_Com_Roc_25AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_16.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_16");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_17, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/17/SM_Com_Roc_26AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_17.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_17");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_18, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/18/SM_Com_Roc_27AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_18.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_18");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_19, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/19/SM_Com_Roc_28AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_19.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_19");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_20, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/20/SM_Com_Roc_29AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_20.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_20");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_21, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/21/SM_Com_Roc_30AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_21.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_21");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_22, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/22/SM_Com_Roc_31AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_22.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_22");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_23, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/23/SM_Com_Roc_32AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_23.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_23");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_24, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/24/SM_Com_Roc_33AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_24.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_24");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_25, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/25/SM_Com_Roc_34AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_25.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_25");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_26, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/26/SM_Com_Roc_35AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_26.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_26");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_27, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/27/SM_Com_Roc_36AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_27.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_27");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_28, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/28/SM_Com_Roc_37AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_28.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_28");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_29, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/29/SM_Com_Roc_38AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_29.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_29");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_30, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/30/SM_Com_Roc_39AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_30.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_30");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_31, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/31/SM_Com_Roc_40AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_31.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_31");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_32, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/32/SM_Com_Roc_41AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_32.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_32");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_33, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/33/SM_Com_Roc_41BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_33.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_33");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_34, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/34/SM_Com_Roc_42AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_34.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_34");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_35, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/35/SM_Com_Roc_43AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_35.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_35");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_36, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/36/SM_Com_Roc_44AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_36.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_36");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_37, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/37/SM_Com_Roc_45AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_37.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_37");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_38, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/38/SM_Com_Roc_49AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_38.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_38");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_39, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/39/SM_Com_Roc_52AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_39.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_39");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_40, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/40/SM_Com_Roc_55AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_40.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_40");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_41, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/41/SM_Com_Roc_66AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_41.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_41");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_42, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/42/SM_Com_Roc_67AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_42.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_42");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_43, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/43/SM_Com_Roc_68AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_43.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_43");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_44, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/44/SM_Com_Roc_70AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_44.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_44");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_45, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/45/SM_Com_Roc_71AH.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_45.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_45");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_46, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/46/SM_Com_Roc_72AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_46.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_46");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_47, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/47/SM_Com_Roc_76AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_47.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_47");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_48, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/48/SM_Com_Roc_77AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_48.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_48");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_49, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/49/SM_Com_Roc_78AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_49.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_49");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_50, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/50/SM_Com_Roc_80AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_50.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_50");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_51, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/51/SM_Com_Roc_82AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_51.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_51");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_ROCK_52, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Rocks/52/SM_Com_Roc_83AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_52.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_ROCK_52");
		return E_FAIL;
	}
#pragma endregion ROCK

#pragma region FLOOR
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_FLOOR_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Floors/0/SM_Alt_Flo_01AH.smdl"), TEXT("../../Data/GamePlay/MapObject/Floors/Floor_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_FLOOR_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_FLOOR_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Floors/1/SM_Alt_Flo_03AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Floors/Floor_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_FLOOR_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_FLOOR_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Floors/2/SM_Alt_Flo_04AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Floors/Floor_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_FLOOR_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_FLOOR_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Floors/3/SM_Alt_Flo_05AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Floors/Floor_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_FLOOR_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_FLOOR_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Floors/4/SM_Alt_Flo_06AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Floors/Floor_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_FLOOR_4");
		return E_FAIL;
	}
#pragma endregion FLOOR

#pragma region STAIRS
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/0/SM_Alt_Sta_01AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/1/SM_Alt_Sta_01BM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/2/SM_Alt_Sta_01CS.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/3/SM_Alt_Sta_01DM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/4/SM_Alt_Sta_02AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/5/SM_Alt_Sta_02BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/6/SM_Alt_Sta_02CS.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/7/SM_Alt_Sta_03AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/8/SM_Alt_Sta_04AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STAIRS_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Stairs/9/SM_Alt_Sta_05AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STAIRS_9");
		return E_FAIL;
	}
#pragma endregion STAIRS

#pragma region GRASS
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/0/SM_Com_Gra_01AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/1/SM_Com_Gra_02AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/2/SM_Com_Gra_02BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/3/SM_Com_Gra_02CS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/4/SM_Com_Gra_03AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/5/SM_Com_Gra_03BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/6/SM_Com_Gra_04AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/7/SM_Com_Gra_04BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/8/SM_Com_Gra_04CS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/9/SM_Com_Gra_04DS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_9");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_10, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/10/SM_Com_Gra_04ES.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_10.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_10");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_11, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/11/SM_Com_Gra_04FS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_11.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_11");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_12, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/12/SM_Com_Gra_05AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_12.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_12");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_13, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/13/SM_Com_Gra_06AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_13.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_13");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_14, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/14/SM_Com_Gra_07AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_14.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_14");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_15, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/15/SM_Com_Gra_08AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_15.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_15");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_GRASS_16, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Grass/16/SM_Com_Gra_09AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Grass/Grass_16.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_GRASS_16");
		return E_FAIL;
	}
#pragma endregion GRASS

#pragma region VIN
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/0/SM_Com_Vin_02AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/1/SM_Com_Vin_03AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/2/SM_Com_Vin_04AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/3/SM_Com_Vin_05AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/4/SM_Com_Vin_06AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/5/SM_Com_Vin_07AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/6/SM_Com_Vin_08AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/7/SM_Com_Vin_09AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/8/SM_Com_Vin_10AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/9/SM_Com_Vin_11AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_9");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_10, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/10/SM_Com_Vin_12AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_10.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_10");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VIN_11, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Vin/11/SM_Com_Vin_13AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Vin/Vin_11.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VIN_11");
		return E_FAIL;
	}
#pragma endregion VIN

#pragma region VEG
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/0/SM_Com_Veg_01AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/1/SM_Com_Veg_02AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/2/SM_Com_Veg_03AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/3/SM_Com_Veg_04AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/4/SM_Com_Veg_05AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/5/SM_Com_Veg_09AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/6/SM_Com_Veg_10AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/7/SM_Com_Veg_11AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/8/SM_Com_Veg_12AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/9/SM_Com_Veg_13AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_9");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_10, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/10/SM_Com_Veg_16AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_10.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_10");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_11, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/11/SM_Com_Veg_18AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_11.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_11");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_12, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/12/SM_Com_Veg_18BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_12.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_12");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_13, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/13/SM_Com_Veg_19AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_13.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_13");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_14, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/14/SM_Com_Veg_20AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_14.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_14");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_15, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/15/SM_Com_Veg_21BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_15.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_15");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_16, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/16/SM_Com_Veg_22AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_16.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_16");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_17, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/17/SM_Com_Veg_23AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_17.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_17");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_18, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/18/SM_Com_Veg_24AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_18.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_18");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_19, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/19/SM_Com_Veg_25AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_19.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_19");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_20, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/20/SM_Com_Veg_26AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_20.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_20");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_21, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/21/SM_Com_Veg_27AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_21.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_21");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_22, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/22/SM_Com_Veg_28AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_22.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_22");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_23, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/23/SM_Com_Veg_29AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_23.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_23");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_24, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/24/SM_Com_Veg_30AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_24.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_24");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_25, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/25/SM_Com_Veg_31BS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_25.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_25");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_26, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/26/SM_Com_Veg_32AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_26.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_26");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_27, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/27/SM_Com_Veg_33AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_27.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_27");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_28, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/28/SM_Com_Veg_34AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_28.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_28");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_29, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/29/SM_Com_Veg_37AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_29.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_29");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_30, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/30/SM_Com_Veg_38AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_30.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_30");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_31, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/31/SM_Com_Veg_42AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_31.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_31");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_VEG_32, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Veg/32/SM_Com_Veg_44AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Veg/Veg_32.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_VEG_32");
		return E_FAIL;
	}
#pragma endregion VEG

#pragma region SHR
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/0/SM_Com_Shr_01AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/1/SM_Com_Shr_02AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/2/SM_Com_Shr_03AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/3/SM_Com_Shr_04AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_3");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_4, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/4/SM_Com_Shr_05AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_4.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_4");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_5, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/5/SM_Com_Shr_05FS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_5.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_5");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_6, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/6/SM_Com_Shr_06AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_6.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_6");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_7, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/7/SM_Com_Shr_07AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_7.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_7");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_8, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/8/SM_Com_Shr_08AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_8.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_8");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_SHR_9, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Shr/9/SM_Com_Shr_09AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Shr/Shr_9.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_SHR_9");
		return E_FAIL;
	}
#pragma endregion SHR

#pragma region STRUCTURE
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STRUCTURE_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Structure/0/SM_Val_Box_01AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Structure/Structure_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STRUCTURE_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STRUCTURE_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Structure/1/SM_Val_Box_01BM.smdl"), TEXT("../../Data/GamePlay/MapObject/Structure/Structure_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STRUCTURE_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STRUCTURE_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Structure/2/SM_Val_Box_02AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Structure/Structure_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STRUCTURE_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STRUCTURE_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Structure/3/SM_Zan_Sta_01AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Structure/Structure_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STRUCTURE_3");
		return E_FAIL;
	}
#pragma endregion STRUCTURE

#pragma region PIL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_PIL_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Pil/0/SM_Val_Pil_01AM.smdl"), TEXT("../../Data/GamePlay/MapObject/Pil/Pil_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_PIL_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_PIL_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Pil/1/SM_Val_Pil_02AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Pil/Pil_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_PIL_1");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_PIL_2, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Pil/2/SM_Val_Pil_03AS.smdl"), TEXT("../../Data/GamePlay/MapObject/Pil/Pil_2.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_PIL_2");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_PIL_3, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Pil/3/SM_Noi_Pil_02BH.smdl"), TEXT("../../Data/GamePlay/MapObject/Pil/Pil_3.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_PIL_3");
		return E_FAIL;
	}
#pragma endregion PIL

#pragma region STATUE
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STATUE_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Statue/0/SM_Val_Sta_02AH.smdl"), TEXT("../../Data/GamePlay/MapObject/Statue/Statue_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STATUE_0");
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SIMODEL::SIMD_STATUE_1, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Statue/1/SM_Val_Sta_02BL.smdl"), TEXT("../../Data/GamePlay/MapObject/Statue/Statue_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : SIMD_STATUE_1");
		return E_FAIL;
	}
#pragma endregion STATUE

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SKY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/SM_SkysphereFlat2.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SKY_LOBBY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/Sky_Lobby.smdl")))))
		return E_FAIL;

#pragma region TERRAIN

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, COMPONENT::VIBUFFER_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Height7.bmp"), TEXT("../../Data/GamePlay/Terrain/Height_Map/Vertices.data")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_D_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Gra_01_D.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_D_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Roc_01A_D.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_D_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_16_D.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_D_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Gra_02_D.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_N_1,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Gra_01_N.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_N_2,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Roc_01A_N.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_N_3,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_San_16_N.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_N_4,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Diffuse/T4_Gra_02_N.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXTURE::TERRAIN_FILTER,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Data/GamePlay/Terrain/Filter_Map/Filter.dds")))))
		return E_FAIL;

#pragma endregion TERRAIN

	m_pApp->LoadRatio(0.9f);
	m_szLoadingStateText = L"���̴��� �ε����Դϴ�.";

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

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYER_CAMERA, CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYER_PLAYERGIRL, CP_PlayerGirl::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MISSILE, CMissile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MISSILE_CONSTANT, CMissile_Constant::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MISSILE_ROTAROUND, CMissile_RotAround::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(OBJECT::SANDBAG, CSandbag::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MONSTER_GAZIZI, CM_GAzizi::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MONSTER_ANJIN, CM_Anjin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MONSTER_AWUKAKA, CM_AWukaka::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SWORD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SWORD))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SCABBARD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SCABBARD))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_0, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_0))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_1, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MAP_OBJECT, CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHEST_SIMPLE, CChest::Create(m_pDevice, m_pContext, CChest::CHEST_SIMPLE))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHEST_STANDARD, CChest::Create(m_pDevice, m_pContext, CChest::CHEST_STANDARD))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHEST_EXPANDED, CChest::Create(m_pDevice, m_pContext, CChest::CHEST_EXPANDED))))
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
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::UITIP, CUI_Tip::Create(m_pDevice, m_pContext))))
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
	m_szLoadingStateText = L"�ؽ��ĸ� �ε����Դϴ�.";

	m_pApp->LoadRatio(0.1f);
	m_szLoadingStateText = L"�������۸� �ε����Դϴ�.";

	m_pApp->LoadRatio(0.3f);
	m_szLoadingStateText = L"�𵨸� �ε����Դϴ�.";

	// SModel
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_GUN_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Gun01/Gun01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SMODEL::SMD_HULU_2, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu03/Hulu03.smdl")))))
		return E_FAIL;

	// DModel
	// Playergirl == nvzhu == �����
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;
	m_pApp->LoadRatio(0.5f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_UI_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_UI_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_UI_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_UI_Ribbon.dmdl")))))
		return E_FAIL;

	// yangyang == ���
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_YANGYANG_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/yangyang/yangyang.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_YANGYANG_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/yangyang/yangyang_AnimSet_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_YANGYANG_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/yangyang/yangyang_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_YANGYANG_UI_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/yangyang/yangyang_UI_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_YANGYANG_UI_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/yangyang/yangyang_UI_Ribbon.dmdl")))))
		return E_FAIL;

	// chixia == �ֱ��ѳ� // AimAttack, AirAttack, HoldShot �� ����
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_CHIXIA_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/chixia/chixia.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_CHIXIA_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/chixia/chixia_AnimSet_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_CHIXIA_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/chixia/chixia_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_CHIXIA_UI_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/chixia/chixia_UI_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_CHIXIA_UI_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/chixia/chixia_UI_Ribbon.dmdl")))))
		return E_FAIL;


	// 4�Ӽ� ���� ���� ���͵�
	// GAzizi == ����� == ���Ӽ�, AWukaka == ���� ������ == �ϼӼ�, FHuxiuxiu == ǳ�Ӽ�, HKacaca == ȭ�Ӽ� 

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_GAZIZI, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Spirit/GAzizi.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_AWUKAKA, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Spirit/AWukaka.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_FHUXIUXIU, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Spirit/FHuxiuxiu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_HKACACA, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Spirit/HKacaca.dmdl")))))
		return E_FAIL;

	// �ΰ��� ����? ���� ���͵�
	// ANJIN == ����, HUOJIN == Į��, BingLie == ����â��, FengLie == ��ǳ���, LeiLie == ����Ȱ��
	// LeiFa == ���� ������, QunJing == ���� ��
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_ANJIN, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Anjin.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_HUOJIN, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Huojin.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_BINGLIE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Binglie.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_FENGLIE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Fenglie.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_LEILIE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Leilie.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_LEIFA, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Leifa.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_QUNJING, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Qunjing.dmdl")))))
		return E_FAIL;

	// ���� ����
	// Binlang1~2 == �� ����(�������), Binlang Elite == ���� ����
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_BINLANG_01, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Wolf/Binlang01.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_BINLANG_02, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Wolf/Binlang02.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_BINLANG_ELITE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Wolf/BinlangElite.dmdl")))))
		return E_FAIL;

	// ������
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_CROWNLESS_P1, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Boss/Crownless/Crownless_P1.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_CROWNLESS_P2, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Boss/Crownless/Crownless_P2.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_CROWNLESS_P3, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Boss/Crownless/Crownless_P3.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_MONSTER_INFERNORIDER, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Boss/InfernoRider/InfernoRider.dmdl")))))
		return E_FAIL;

	m_pApp->LoadRatio(0.6f);
	m_szLoadingStateText = L"���̴��� �ε����Դϴ�.";
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
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_NULL, CTestGeneric::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/


	//VTF
	CTestVTF::TESTVTF_DESC tDesc;
	// PLAYERGIRL
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_PLAYERGIRL_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_SWORD_0_SWORD;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_SWORD_0_SCABBARD;
	tDesc.iPartsID[CTestVTF::PARTS_HULU] = OBJECT::PARTS_HULU_2;
	lstrcpy(tDesc.szFilePath, TEXT("../../Data/CharState/P_PlayerGirl/PlayerGirl_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_PLAYERGIRL, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_PLAYERGIRL), TEXT("PlayerGirl"));

	// PLAYERGIRL_UI
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_PLAYERGIRL_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_PLAYERGIRL_UI_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_PLAYERGIRL_UI_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_SWORD_0_SWORD;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_SWORD_0_SCABBARD;
	tDesc.iPartsID[CTestVTF::PARTS_HULU] = OBJECT::PARTS_HULU_2;
	lstrcpy(tDesc.szFilePath, TEXT("../../Data/CharState/P_PlayerGirl_UI/PlayerGirl_UI_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_PLAYERGIRL_UI, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_PLAYERGIRL_UI), TEXT("PlayerGirl_UI"));

	// YANGYANG
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_YANGYANG_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_YANGYANG_ANIMSET_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_YANGYANG_ANIMSET_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_SWORD_0_SWORD;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_SWORD_0_SCABBARD;
	tDesc.iPartsID[CTestVTF::PARTS_HULU] = OBJECT::PARTS_HULU_0;
	lstrcpy(tDesc.szFilePath, TEXT("../../Data/CharState/P_YangYang/YangYang_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_YANGYANG, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_YANGYANG), TEXT("YangYang"));

	// YANGYANG_UI
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_YANGYANG_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_YANGYANG_UI_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_YANGYANG_UI_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_SWORD_0_SWORD;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_SWORD_0_SCABBARD;
	tDesc.iPartsID[CTestVTF::PARTS_HULU] = OBJECT::PARTS_HULU_0;
	lstrcpy(tDesc.szFilePath, TEXT("../../Data/CharState/P_YangYang_UI/YangYang_UI_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_YANGYANG_UI, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_YANGYANG_UI), TEXT("YangYang_UI"));

	// CHIXIA
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_CHIXIA_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_CHIXIA_ANIMSET_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_CHIXIA_ANIMSET_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_GUN_0;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_GUN_0;
	tDesc.iPartsID[CTestVTF::PARTS_HULU] = OBJECT::PARTS_HULU_1;
	lstrcpy(tDesc.szFilePath, TEXT("../../Data/CharState/P_ChiXia/ChiXia_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_CHIXIA, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_CHIXIA), TEXT("ChiXia"));

	// CHIXIA_UI
	ZeroMemory(&tDesc, sizeof tDesc);
	tDesc.iModelID = DMODEL::DMD_CHIXIA_MODEL;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_BASE] = DMODEL::DMD_CHIXIA_UI_BASE;
	tDesc.iAnimSetID[CTestVTF::ANIMSET_RIBBON] = DMODEL::DMD_CHIXIA_UI_RIBBON;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_MAIN] = OBJECT::PARTS_GUN_0;
	tDesc.iPartsID[CTestVTF::PARTS_WEAPON_SUB] = OBJECT::PARTS_GUN_0;
	tDesc.iPartsID[CTestVTF::PARTS_HULU] = OBJECT::PARTS_HULU_1;
	lstrcpy(tDesc.szFilePath, TEXT("../../Data/CharState/P_ChiXia_UI/ChiXia_UI_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTVTF_CHIXIA_UI, CTestVTF::Create(m_pDevice, m_pContext, &tDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_VTF, VTFID(OBJECT::TESTVTF_CHIXIA_UI), TEXT("ChiXia_UI"));


	CTestGeneric::TESTGENERIC_DESC tGenericDesc;
	//GENERIC
	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_GAZIZI;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_GAzizi/GAzizi_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_GAZIZI, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_GAZIZI), TEXT("GAzizi"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_AWUKAKA;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_AWukaka/AWukaka_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_AWUKAKA, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_AWUKAKA), TEXT("AWukaka"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_HKACACA;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_HKacaca/HKacaca_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_HKACACA, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_HKACACA), TEXT("HKacaca"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_FHUXIUXIU;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_FHuxiuxiu/FHuxiuxiu_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_FHUXIUXIU, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_FHUXIUXIU), TEXT("FHuxiuxiu"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_ANJIN;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Anjin/Anjin_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_ANJIN, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_ANJIN), TEXT("Anjin"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_HUOJIN;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Huojin/Huojin_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_HUOJIN, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_HUOJIN), TEXT("Huojin"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_BINGLIE;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Binglie/Binglie_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_BINGLIE, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_BINGLIE), TEXT("Binglie"));


	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_FENGLIE;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Fenglie/Fenglie_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_FENGLIE, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_FENGLIE), TEXT("Fenglie"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_LEILIE;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Leilie/Leilie_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_LEILIE, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_LEILIE), TEXT("Leilie"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_LEIFA;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Leifa/Leifa_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_LEIFA, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_LEIFA), TEXT("Leifa"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_QUNJING;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Qunjing/Qunjing_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_QUNJING, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_QUNJING), TEXT("Qunjing"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_BINLANG_01;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Binlang/Binlang_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_BINLANG_01, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_BINLANG_01), TEXT("Binlang01"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_BINLANG_02;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Binlang/Binlang_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_BINLANG_02, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_BINLANG_02), TEXT("Binlang02"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_BINLANG_ELITE;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_BinlangElite/BinlangElite_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_BINLANG_ELITE, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_BINLANG_ELITE), TEXT("BinlangElite"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_CROWNLESS_P1;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Crownless_P1/M_Crownless_P1_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_CROWNLESS_P1, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_CROWNLESS_P1), TEXT("Crownless_P1"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_CROWNLESS_P2;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Crownless_P2/M_Crownless_P2_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_CROWNLESS_P2, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_CROWNLESS_P2), TEXT("Crownless_P2"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_CROWNLESS_P3;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_Crownless_P3/M_Crownless_P3_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_CROWNLESS_P3, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_CROWNLESS_P3), TEXT("Crownless_P3"));

	ZeroMemory(&tGenericDesc, sizeof tGenericDesc);
	tGenericDesc.iModelID = DMODEL::DMD_MONSTER_INFERNORIDER;
	lstrcpy(tGenericDesc.szFilePath, TEXT("../../Data/CharState/M_InfernoRider/M_InfernoRider_%d.state"));

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TESTGENERIC_INFERNORIDER, CTestGeneric::Create(m_pDevice, m_pContext, &tGenericDesc))))
		return E_FAIL;
	pAnimTool->Add_ListBoxItem(CAnimToolManager::MODEL_GENERIC, GENERICID(OBJECT::TESTGENERIC_INFERNORIDER), TEXT("InfernoRider"));




	// GENERIC ��

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SWORD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SWORD))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SCABBARD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SCABBARD))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_GUN_0, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_GUN_0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_0, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_1, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_2, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_2))))
		return E_FAIL;

#pragma endregion

	m_szLoadingStateText = L"Load Completed";
	m_isFinish = true;
	return S_OK;
}

HRESULT CLoader::Load_Level_Test()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//GamePlay Component
#pragma region COMPONENTS
	m_szLoadingStateText = L"�ؽ��ĸ� �ε����Դϴ�.";
	// UI
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::UI,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Main/main%d.dds"), 255))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::UIMOUSE,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Mouse/CursorPre%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::UIMAP,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/Map%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::UIMAPDEFAULT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/DefaultIcon%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::UIMAPICON,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Map/Icon%d.dds"), 47))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::UIFIGHT,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Fight/Fight%d.dds"), 79))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::EYE_BURST,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeBurst.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::EYE_MASK,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/EyeMask.dds"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::BOX_EMISSIVE,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Static/Prop/Interaction/SimpleBox/box_emissive.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::BOX_SPECULAR,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Static/Prop/Interaction/SimpleBox/box_spec.dds"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, TEXTURE::BOX_SSAO,
		CTexture::Create(m_pDevice, m_pContext, L"../../Resource/Model/Static/Prop/Interaction/SimpleBox/box_ssao.dds"))))
		return E_FAIL;

	m_pApp->LoadRatio(0.1f);
	m_szLoadingStateText = L"�������۸� �ε����Դϴ�.";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, COMPONENT::NAVIGATION,
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/Test/Navigation.data")))))
		return E_FAIL;

	m_pApp->LoadRatio(0.2f);
	m_szLoadingStateText = L"�𵨸� �ε����Դϴ�.";

	// SMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_HULU_2, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu03/Hulu03.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_SIMPLE_BOX, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Interaction/SimpleBox/SimpleBox.smdl")))))
		return E_FAIL;

	// DMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;
	m_pApp->LoadRatio(0.6f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, DMODEL::DMD_MONSTER_ANJIN, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/Monster/Common/Ghost/Anjin.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_SKY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/SM_SkysphereFlat2.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SMODEL::SMD_SKY_LOBBY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/Sky_Lobby.smdl")))))
		return E_FAIL;

	m_pApp->LoadRatio(0.9f);
	m_szLoadingStateText = L"���̴��� �ε����Դϴ�.";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TEST, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	m_pApp->LoadRatio(1.f);
	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYER_CAMERA, CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYER_PLAYERGIRL, CP_PlayerGirl::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MISSILE, CMissile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MISSILE_CONSTANT, CMissile_Constant::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MISSILE_ROTAROUND, CMissile_RotAround::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(OBJECT::SANDBAG, CSandbag::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SWORD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SWORD))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_SWORD_0_SCABBARD, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_SWORD_0_SCABBARD))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_0, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_0))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_1, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PARTS_HULU_2, CParts::Create(m_pDevice, m_pContext, SMODEL::SMD_HULU_2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::MAP_OBJECT, CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHEST_SIMPLE, CChest::Create(m_pDevice, m_pContext, CChest::CHEST_SIMPLE))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHEST_STANDARD, CChest::Create(m_pDevice, m_pContext, CChest::CHEST_STANDARD))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::CHEST_EXPANDED, CChest::Create(m_pDevice, m_pContext, CChest::CHEST_EXPANDED))))
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
