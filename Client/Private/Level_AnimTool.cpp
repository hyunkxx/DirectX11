#include "stdafx.h"
#include "..\Public\Level_AnimTool.h"

#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "Character.h"

#include "AnimToolManager.h"

CLevel_AnimTool::CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_AnimTool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_AnimTool::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance();
	pAppManager->SetTitle(L"LEVEL_ANIMTOOL");
#endif

}

void CLevel_AnimTool::RenderLevelUI()
{
	CAnimToolManager::GetInstance()->RenderGUI();
}

HRESULT CLevel_AnimTool::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHT_DESC LightDesc;

	// Directional
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANIMTOOL, OBJECT::TERRAIN, pLayerTag, L"terrain")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 20.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	CameraDesc.vEye = _float3(0.f, 1.2f, -2.5f);
	CameraDesc.vAt = _float3(0.f, 1.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANIMTOOL, OBJECT::DYNAMIC_CAMERA, pLayerTag, L"dynamic_camera", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANIMTOOL, OBJECT::TESTGENERIC, pLayerTag, TEXT("TestPlayer0"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANIMTOOL, OBJECT::TEST_WUTHERING, pLayerTag, TEXT("TestPlayer1"))))
	//	return E_FAIL;


	return S_OK;
}

CLevel_AnimTool* CLevel_AnimTool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_AnimTool* pInstance = new CLevel_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_AnimTool";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_AnimTool::Free()
{
	__super::Free();
}
