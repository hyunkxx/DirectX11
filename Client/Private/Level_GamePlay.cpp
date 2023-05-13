#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"
#include "Character.h"
#include "PlayerGirl.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Off
	pGameInstance->SetShadowLevel((CRenderSetting::SHADOW_LEVEL)SHADOW_LEVEL::SHADOW_HIGH);
	pGameInstance->OutlineToggle();
	pGameInstance->SSAOToggle();

	if(FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_UI(TEXT("layer_UI"))))
		return E_FAIL;

#pragma region MAP_OBJECT_TREE
	if (FAILED(Ready_Layer_MapObject_Tree(TEXT("layer_Tree"))))
		return E_FAIL;
#pragma endregion MAP_OBJECT_TREE

	pGameInstance->StartFade(CRenderSetting::FADE_IN, 4.f);

	pGameInstance->SetVolume(SOUND_TYPE::SOUND_BGM, 0.5f);
	pGameInstance->PlaySoundEx(L"Base_BGM.mp3", SOUND_CHANNEL::BGM, VOLUME_BGM);

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance(); 
	pAppManager->SetTitle(L"LEVEL_GAMEPLAY");
#endif

	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//pGameInstance->BGMSmoothOn(TimeDelta);

	//카메라 기준으로 쉐도우 조명 갱신
	pGameInstance->ShadowUpdate(120.f);

	//임시 그래픽 세팅 추후에 시스템 UI만들면서 넣을것
	static _uint iShadowLevel = 0;
	if (pGameInstance->InputKey(DIK_0) == KEY_STATE::TAP)
	{
		iShadowLevel++;
		if (iShadowLevel > 2)
			iShadowLevel = 0;

		pGameInstance->SetShadowLevel((CRenderSetting::SHADOW_LEVEL)iShadowLevel);
	}

	if (pGameInstance->InputKey(DIK_PAUSE) == KEY_STATE::TAP)
	{
		pGameInstance->DebugTargetToggle();
	}

	if (pGameInstance->InputKey(DIK_0) == KEY_STATE::TAP)
	{
		iShadowLevel++;
		if (iShadowLevel > 2)
			iShadowLevel = 0;

		pGameInstance->SetShadowLevel((CRenderSetting::SHADOW_LEVEL)iShadowLevel);
	}

	if (pGameInstance->InputKey(DIK_9) == KEY_STATE::TAP)
		pGameInstance->OutlineToggle();

	if (pGameInstance->InputKey(DIK_8) == KEY_STATE::TAP)
		pGameInstance->SSAOToggle();

	static _uint iIndexLUT = CRenderer::LUT_DEFAULT;
	if (pGameInstance->InputKey(DIK_7) == KEY_STATE::TAP)
	{
		iIndexLUT++;
		if(iIndexLUT >= CRenderer::LUT_DEFAULT + 1)
			iIndexLUT = 0;

		pGameInstance->SetLUT((CRenderer::LUT)iIndexLUT);
	}


	if (pGameInstance->InputKey(DIK_LCONTROL) == KEY_STATE::HOLD &&
		pGameInstance->InputKey(DIK_6) == KEY_STATE::TAP)
	{
		pGameInstance->StartBlackWhite(0.05f);
	}
	else if (pGameInstance->InputKey(DIK_6) == KEY_STATE::TAP)
	{
		pGameInstance->StartBlackWhite(1.f);
	}

	if (pGameInstance->InputKey(DIK_LCONTROL) == KEY_STATE::HOLD &&
		pGameInstance->InputKey(DIK_5) == KEY_STATE::TAP)
	{
		pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 0.5f);
	}
	else if (pGameInstance->InputKey(DIK_5) == KEY_STATE::TAP)
	{
		pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 2.f);
	}

	//static _bool bCamLock = false;
	//if (pGameInstance->InputKey(DIK_SCROLL) == KEY_STATE::TAP)
	//{
	//	bCamLock = !bCamLock;

	//	if (bCamLock)
	//		pGameMode->UseCamera(CAM_DYNAMIC);
	//	else
	//		pGameMode->UseCamera(CAM_PLAYER);
	//}
}

void CLevel_GamePlay::RenderLevelUI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	ImGui::Begin("Helper");

	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Frame Rate");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	string strCamSpeed = to_string(int(m_pDynamicCamera->GetCameraSpeed()));
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "DynamicCamera");
	ImGui::Text("Current CamSpeed : x"); ImGui::SameLine();
	ImGui::Text(strCamSpeed.c_str());
	ImGui::Text("Ctrl+PageUp    : Speed Up");
	ImGui::Text("Ctrl+PageDown  : Speed Down");

	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "RenderSetting");
	ImGui::Text("[ 0 ] Shadow      (Off/Low/High)");
	ImGui::Text("[ 9 ] Outline     (On/Off)");
	ImGui::Text("[ 8 ] SSAO        (On/Off)");
	ImGui::Text("[ 7 ] LUT Change  "); ImGui::SameLine();

	switch (pGameInstance->GetLUT())
	{
	case CRenderer::LUT_EXPOSURE_MINUS:
		ImGui::Text("Exposure Minus");
		break;
	case CRenderer::LUT_EXPOSURE_PLUS:
		ImGui::Text("Exposure Plus");
		break;
	case CRenderer::LUT_FUJI:
		ImGui::Text("Fuji");
		break;
	case CRenderer::LUT_GRUNGY:
		ImGui::Text("Grungy");
		break;
	case CRenderer::LUT_SOUTH:
		ImGui::Text("South");
		break;
	case CRenderer::LUT_KURO:
		ImGui::Text("Kuro");
		break;
	case CRenderer::LUT_DEFAULT:
		ImGui::Text("Default");
		break;
	}

	CRenderSetting::RGB_SPLIT_DESC& rgbDesc = pGameInstance->GetSplitDesc();
	ImGui::Text("[ 6 ] [ Ctrl+6]    Black&White ");
	ImGui::Text("[ 5 ] [ Ctrl+5]    RGB Split");
	ImGui::DragFloat3("Split Value", (float*)&rgbDesc);

	ImGui::Text("[   Pause Key   ]	Target On/Off");

	ImGui::End();
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TERRAIN, pLayerTag, L"terrain")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::SKY, pLayerTag, L"Sky")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Camera Setting
	CCamera::CAMERA_DESC CameraDesc;
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
	
	//Light Setting
	_matrix vLightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), CameraDesc.fAspect, CameraDesc.fNear, CameraDesc.fFar);
	pGameInstance->SetLightMatrix(vLightProjMatrix, LIGHT_MATRIX::LIGHT_PROJ);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::DYNAMIC_CAMERA, pLayerTag, L"dynamic_camera", &CameraDesc)))
		return E_FAIL;

	m_pDynamicCamera = (CDynamicCamera*)pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, L"dynamic_camera");

	//PlayerCamera Setting
	CPlayerCamera::PLAYERCAM_DESC PCamDesc;
	ZeroMemory(&PCamDesc, sizeof CPlayerCamera::PLAYERCAM_DESC);
	PCamDesc.CamDesc = CameraDesc;
	// PCamDesc.pPlayerTransform  >> Start에서 채움
	// PCamDesc.vDir Start에서 채움
	PCamDesc.fDistance = 4.f;
	PCamDesc.fXAngle = 20.f;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYER_CAMERA, pLayerTag, L"player_camera", &PCamDesc)))
		return E_FAIL;

	m_pPlayerCamera = (CPlayerCamera*)pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, L"player_camera");

	pGM->PushCamera(m_pDynamicCamera);
	pGM->PushCamera(m_pPlayerCamera);
	
	pGM->UseCamera(CGameMode::CAM_DYNAMIC);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	/*int j = 0;
	_tchar szTag[MAX_TAG];
	wsprintf(szTag, L"Texture%d", j);
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHARACTER, pLayerTag, szTag, &j)))
		return E_FAIL;*/

	CPlayerGirl::Init_States(m_pDevice, m_pContext);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYERGIRL, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UI, pLayerTag, TEXT("UI_MainScreen"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MOUSE, pLayerTag, TEXT("UI_Mouse"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UITAPT, pLayerTag, TEXT("UI_TapT"))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIMONSTER, pLayerTag, TEXT("UI_Monster"))))
	//	return E_FAIL;
	

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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_1, pLayerTag, TEXT("ObjectTree"))))
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

	CPlayerGirl::Release_States();
	
}
