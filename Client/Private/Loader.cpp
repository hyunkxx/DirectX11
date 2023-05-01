#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Terrain.h"
#include "Floor.h"

#include "PlayerGirl.h"

#include "Tree_0.h"


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
	

	m_szLoadingStateText = L"셰이더를 로딩중입니다.";

	

#pragma endregion

	//Logo GameObject
#pragma region GAMEOBJECTS
	
	m_szLoadingStateText = L"객체 원형을 준비중";

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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_MODEL, CModel_VTF::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/nvzhu/nvzhu.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE_POSE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/nvzhu/nvzhu_Base_Pose.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE_ACTION, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/nvzhu/nvzhu_Base_Action.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON_POSE, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/nvzhu/nvzhu_Ribbon_Pose.dmdl")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON_ACTION, CModel_Anim::Create(m_pDevice, m_pContext, TEXT("../../Resource/Model/Dynamic/nvzhu/nvzhu_Ribbon_Action.dmdl")))))
		return E_FAIL;


#pragma region MODEL_INSTANCE_TREE
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SMODEL::SMD_TREE_0, CModel_Instance::Create(m_pDevice, m_pContext,
		TEXT("../../Resource/Model/Static/Map/Object/Trees/0/SM_Com_Tre_02AL.smdl"), TEXT("../../Data/GamePlay/MapObject/Tree_0.data")))))
	{
		MSG_BOX("Failed to Prototype In Loader : MODEL_INSTANCE");
		return E_FAIL;
	}
#pragma endregion MODEL_INSTANCE_TREE


	m_szLoadingStateText = L"셰이더를 로딩중입니다.";
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SHADER::MODEL, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXSMODEL_DECLARATION::ElementDesc, VTXSMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SHADER::MODELANIM, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODELANIM.hlsl"), VTXDMODEL_DECLARATION::ElementDesc, VTXDMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

#pragma region SHADER_MODEL_INSTANCE
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, SHADER::MODEL_INSTANCE, CShader::Create(m_pDevice, m_pContext,
		TEXT("../../Shader/SHADER_VTXMODEL_INSTANCE.hlsl"), VTXSMODELINSTANCE_DECLARATION::Elements, VTXSMODELINSTANCE_DECLARATION::ElementCount))))
	{
		MSG_BOX("Failed to Prototype In Loader : MODEL_INSTANCE_SHADER");
		return E_FAIL;
	}
#pragma endregion SHADER_MODEL_INSTANCE

#pragma endregion

	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::TERRAIN, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::FLOOR, CFloor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(OBJECT::PLAYERGIRL, CPlayerGirl::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region MAP_OBJECT_TREE
	if (FAILED(pGameInstance->Add_Prototype(OBJECT::OBJECT_TREE_0, CTree_0::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion MAP_OBJECT_TREE

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
