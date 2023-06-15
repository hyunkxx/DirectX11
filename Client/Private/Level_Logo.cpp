#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "PlayerState.h"
#include "IntroCamera.h"
#include "DynamicCamera.h"
#include "CameraMovement.h"

#include "Lobby_Character.h"
#include "AcquireSystem.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	LIGHT_DESC LightDesc;

	// Directional
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(2.f, -0.3f, 1.f, 0.f);
	
	//LightDesc.vDiffuse = _float4(0.8f, 0.9f, 0.8f, 1.f);
	//LightDesc.vAmbient = _float4(0.65f, 0.65f, 0.65f, 1.f); //0.65f
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	pGameInstance->SetLUT(CRenderer::LUT_FUJI);
	pGameInstance->SetShadowLevel((CRenderSetting::SHADOW_LEVEL)SHADOW_LEVEL::SHADOW_HIGH);
	pGameInstance->OutlineToggle();
	pGameInstance->SSAOToggle();
	pGameInstance->StartFade(CRenderSetting::FADE_IN, 3.f);

	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Character(TEXT("layer_character"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;
	
	if (FAILED(Ready_StaticGameObject(TEXT("layer_static"))))
		return E_FAIL;

	pGameInstance->SetVolume(SOUND_TYPE::SOUND_BGM, 0.5f);
	pGameInstance->PlaySoundEx(L"Login.mp3", SOUND_CHANNEL::BGM, VOLUME_BGM);
	
	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance();
	pAppManager->SetTitle(L"LEVEL_LOGO");
#endif
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->ShadowUpdate(40.f, XMLoadFloat4(&pGameInstance->Get_CamPosition()));

	if (static_cast<CIntroCamera*>(m_pIntroCam)->IsLobbyOut() || KEY_STATE::TAP == pGameInstance->InputKey(DIK_TAB))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
	else if (static_cast<CIntroCamera*>(m_pIntroCam)->IsLobbyOut() || KEY_STATE::TAP == pGameInstance->InputKey(DIK_F9))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TEST));

#pragma region LEVEL_MOVE
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F1))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F2))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CITY));
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F3))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FOREST));
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F4))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CROWN));
#pragma endregion LEVEL_MOVE

}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

 	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANYWHERE, OBJECT::CHARACTERSELECT_TERRAIN, pLayerTag, L"terrain")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANYWHERE, OBJECT::SKY_LOBBY, pLayerTag, L"sky")))
		return E_FAIL;

	for (int i = 0; i < 30; ++i)
	{
		_tchar szTag[MAX_TAG] = L"";
		wsprintf(szTag, L"rock%d", i);
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::FLOATING_STONE, pLayerTag, szTag)))
			return E_FAIL;
	}

	if (FAILED(Load_LobbyRock(TEXT("../../Data/Lobby/Rock/LobbyRock.data"), pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Camera Setting
	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 0.2f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	CameraDesc.vEye = _float3(123.f, 2.f, 102.f);
	CameraDesc.vAt = _float3(123.f, 2.5f, 112.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	//Light Setting
	_matrix vLightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), CameraDesc.fAspect, CameraDesc.fNear, CameraDesc.fFar);
	pGameInstance->SetLightMatrix(vLightProjMatrix, LIGHT_MATRIX::LIGHT_PROJ);

	// Camera Settings
	CGameObject* pCamera = nullptr;
	CGameObject* pCamMovement = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectEx(&m_pIntroCam, LEVEL_LOGO, OBJECT::INTRO_CAMERA, pLayerTag, L"IntroCamera", &CameraDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectEx(&pCamMovement, LEVEL_STATIC, OBJECT::STATIC_CAM_MOVEMENT, pLayerTag, L"CameraMovement")))
		return E_FAIL;

	//Camera Setting
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 5.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(45.f);

	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(10.f, 3.f, 10.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	// Dynamic 캠 생성 등록
	if (FAILED(pGameInstance->Add_GameObjectEx(&pCamera, LEVEL_STATIC, OBJECT::DYNAMIC_CAMERA, pLayerTag, L"DynamicCamera", &CameraDesc)))
		return E_FAIL;
	if (FAILED(static_cast<CCameraMovement*>(pCamMovement)->AddCamera(CCameraMovement::CAM_DYNAMIC, static_cast<CCamera*>(pCamera))))
		return E_FAIL;
	
	// Action 캠 생성 : Action Cam 자동으로 등록됨
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::ACTION_CAM_BANGSUN, pLayerTag, L"ActionCam", &CameraDesc)))
		return E_FAIL;

	CameraDesc.vEye = _float3(-1000.3f, 1.2f, -999.7f);
	CameraDesc.vAt = _float3(-999.5f, 1.2f, -999.7f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	// UI 캠 생성 , 등록
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::UI_CAM, pLayerTag, L"UICam", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Character(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::STATIC_CHARACTER_STATE, L"StaticObject", TEXT("CharacterState"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::LOBBY_CHARACTER_LEFT, pLayerTag, L"LobbyCharacter_Left")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::LOBBY_CHARACTER_RIGHT, pLayerTag, L"LobbyCharacter_Right")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_StaticGameObject(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameMode* pGameMode = CGameMode::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::ECHO_SYSTEM, L"EchoSystem", L"Echo")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::INVENTORY, pLayerTag, L"Inventory")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::UI_TERMINAL, L"Terminal_UI", L"Terminal")))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::ACQUIRE_SYSTEM, pLayerTag, TEXT("AcquireSystem"))))
		return E_FAIL;

	// 에코
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, OBJECT::ECHO_ANJIN, L"EchoInstance", L"Echo_Anjin")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Load_LobbyRock(const _tchar* pLobbyRockFilePath, const _tchar * pLayerTag)
{
	HANDLE		hFile = CreateFile(pLobbyRockFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in Logo : LobbyRock");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;


	DWORD		dwByte = 0;
	_uint		iLobbyRockCount = { 0 };

	ReadFile(hFile, &iLobbyRockCount, sizeof(_uint), &dwByte, nullptr);

	SMAP_OBJECT_DESC		sMapObjectDesc = {};

	for (_uint i = 0; i < iLobbyRockCount; ++i)
	{
		ZeroMemory(&sMapObjectDesc, sizeof(SMAP_OBJECT_DESC));

		ReadFile(hFile, &sMapObjectDesc.SObjectDesc.vP, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &sMapObjectDesc.SObjectDesc.vS, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &sMapObjectDesc.SObjectDesc.vA, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &sMapObjectDesc.iModelTypeID, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &sMapObjectDesc.iDiffuseTexID, sizeof(_uint), &dwByte, nullptr);

		_tchar szLobbyRockTag[MAX_TAG] = L"";
		wsprintf(szLobbyRockTag, TEXT("lobbyrock_%d"), i);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::LOBBY_ROCK, pLayerTag, szLobbyRockTag, &sMapObjectDesc)))
			return E_FAIL;

	}

	CloseHandle(hFile);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_Logo";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
