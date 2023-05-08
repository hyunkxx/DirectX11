#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "IntroCamera.h"
#include "DynamicCamera.h"
#include "Lobby_Character.h"

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
	LightDesc.vDirection = _float4(2.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.68f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//On
	//pGameInstance->SetLUT(CRenderer::LUT_KURO);
	pGameInstance->SetShadowLevel((CRenderSetting::SHADOW_LEVEL)SHADOW_LEVEL::SHADOW_HIGH);
	pGameInstance->OutlineToggle();
	pGameInstance->SSAOToggle();

	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Character(TEXT("layer_character"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance();
	pAppManager->SetTitle(L"LEVEL_LOGO");
#endif
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->ShadowUpdate(40.f);

	if (pGameInstance->InputKey(DIK_RETURN) == KEY_STATE::TAP)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

 	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANYWHERE, OBJECT::FLOOR, pLayerTag, L"terrain")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANYWHERE, OBJECT::SKY, pLayerTag, L"sky")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Camera Setting
	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 5.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	CameraDesc.vEye = _float3(0.f, 1.f, -4.f);
	CameraDesc.vAt = _float3(0.f, 1.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;

	//Light Setting
	_matrix vLightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), CameraDesc.fAspect, CameraDesc.fNear, CameraDesc.fFar);
	pGameInstance->SetLightMatrix(vLightProjMatrix, LIGHT_MATRIX::LIGHT_PROJ);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::INTRO_CAMERA, pLayerTag, L"IntroCamera", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Character(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::LOBBY_CHARACTER_LEFT, pLayerTag, L"LobbyCharacter_Left")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, OBJECT::LOBBY_CHARACTER_RIGHT, pLayerTag, L"LobbyCharacter_Right")))
		return E_FAIL;

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
