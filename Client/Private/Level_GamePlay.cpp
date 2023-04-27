#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "Character.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if(FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;
	
#pragma region MAP_OBJECT_TREE
	if (FAILED(Ready_Layer_MapObject_Tree(TEXT("layer_Tree"))))
		return E_FAIL;
#pragma endregion MAP_OBJECT_TREE

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance(); 
	pAppManager->SetTitle(L"LEVEL_GAMEPLAY");
#endif

	//카메라 기준으로 쉐도우 조명 갱신
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->ShadowUpdate();

}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHT_DESC LightDesc;

	// Directional
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(2.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.65f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	
	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TERRAIN, pLayerTag, L"terrain")))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::FLOOR, pLayerTag, L"floor")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Camera Setting
	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 30.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(10.f, 3.f, 10.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	
	//Light Setting

	_matrix vLightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), CameraDesc.fAspect, CameraDesc.fNear, CameraDesc.fFar);
	pGameInstance->SetLightMatrix(vLightProjMatrix, LIGHT_MATRIX::LIGHT_PROJ);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::DYNAMIC_CAMERA, pLayerTag, L"dynamic_camera", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	int i = 0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHARACTER, pLayerTag, L"Texture", &i)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TESTPLAYER, pLayerTag, TEXT("TestPlayer"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TESTPLAYER, pLayerTag, TEXT("TestPlayer2"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Tree(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == pGameInstance)
		return E_FAIL;
	;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_0, pLayerTag, TEXT("ObjectTree"))))
	{
		MSG_BOX("Failed to AddGameObject In GamePlay : ObjectTree");
		return E_FAIL;
	}

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_GamePlay";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
