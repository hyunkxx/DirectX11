#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Terrain.h"
#include "Floor.h"

#include "Lobby_Character.h"
#include "PlayerGirl.h"
#include "Parts.h"

#include "PlayerCamera.h"

#include "Tree_0.h"
#include "Tree_1.h"

#include "Sky.h"

//AnimTool
#include "AnimToolManager.h"
#include "TestVTF.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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

	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";

	m_szLoadingStateText = L"모델를 로딩중입니다.";
	// SMODEL
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SWORD_0_SWORD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Sword.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SWORD_0_SCABBARD, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Sword01/Sword01_Scabbard.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_HULU_0, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu01/Hulu01.smdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_HULU_1, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Prop/Player/Hulu02/Hulu02.smdl")))))
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

	m_szLoadingStateText = L"셰이더를 로딩중입니다.";
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, SMODEL::SMD_SKY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/SM_SkysphereFlat2.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader : Sky");
		return E_FAIL;
	}

#pragma endregion

	//Logo GameObject
#pragma region GAMEOBJECTS
	
	m_szLoadingStateText = L"객체 원형을 준비중";
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_CHARACTER_LEFT, CLobbyCharacter::Create(m_pDevice, m_pContext, DMODEL::DMD_PLAYERGIRL_MODEL, DMODEL::DMD_LOBBY_CHARACTER_MODEL, 0))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::LOBBY_CHARACTER_RIGHT, CLobbyCharacter::Create(m_pDevice, m_pContext, DMODEL::DMD_PLAYERGIRL_MODEL, DMODEL::DMD_LOBBY_CHARACTER_MODEL, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::SKY, CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	m_szLoadingStateText = L"Load Completed";
	m_isFinish = true;
	return S_OK;
}

HRESULT CLoader::Load_Level_GamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//GamePlay Component
#pragma region COMPONENTS
	m_szLoadingStateText = L"텍스쳐를 로딩중입니다.";

	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";

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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_TREE_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/0/SM_Com_Tre_01AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Tree_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : Tree_0");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_TREE_1, CModel_Instance_Tree1::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/1/SM_Com_Tre_02AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Tree_1.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : Tree_1");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_SKY, CModel::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Static/Map/Sky/SM_SkysphereFlat2.smdl")))))
	{
		MSG_BOX("Failed to Prototype In Loader : Sky");
		return E_FAIL;
	}
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

	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext))))
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

#pragma endregion



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

	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";

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

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Base.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/PlayerChar/PlayerGirl/nvzhu_AnimSet_Ribbon.dmdl")))))
		return E_FAIL;


	m_szLoadingStateText = L"셰이더를 로딩중입니다.";
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

#pragma endregion

	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext))))
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
