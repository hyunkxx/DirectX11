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
	if (FAILED(Ready_Layer_MapObject_Tree(TEXT("layer_tree"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_MapObject_Rock(TEXT("layer_rock"))))
		//return E_FAIL;
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
	if (nullptr == pGameInstance)
		return E_FAIL;

	SOBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof EditionDesc);

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/Terrain/UVSamplerRatio.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TERRAIN, pLayerTag, L"terrain", &EditionDesc)))
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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::ACQUIRE_SYSTEM, pLayerTag, TEXT("AcquireSystem"))))
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
	
	SOBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof EditionDesc);

	EditionDesc.fCullingRatio = { 30.0f };

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_0_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_0, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_1_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_1, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;

	/*
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_2_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_2, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_3_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_3, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_4_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_4, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_5_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_5, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_6_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_6, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_7_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_7, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_8_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_8, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_9_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_9, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_10_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_10, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_11_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_11, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_12_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_12, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_13_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_13, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_14_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_14, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_15_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_15, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_16_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_16, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_17_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_17, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_18_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_18, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_19_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_19, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_20_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_20, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_21_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_21, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_22_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_22, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_23_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_23, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_24_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_24, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_25_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_25, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_26_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_26, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_27_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_27, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_28_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_28, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_29_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_29, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_30_EditionColor.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_TREE_30, pLayerTag, TEXT("ObjectTree"), &EditionDesc)))
		return E_FAIL;
	*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Rock(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == pGameInstance)
		return E_FAIL;

	SOBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof EditionDesc);

	EditionDesc.fCullingRatio = { 30.0f };

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_0_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_0, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_1_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_1, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_2_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_2, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_3_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_3, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_4_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_4, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_5_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_5, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_6_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_6, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_7_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_7, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_8_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_8, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_9_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_9, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_10_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_10, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_11_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_11, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_12_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_12, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_13_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_13, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_14_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_14, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_15_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_15, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_16_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_16, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_17_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_17, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_18_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_18, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_19_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_19, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_20_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_20, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_21_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_21, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_22_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_22, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_23_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_23, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_24_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_24, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_25_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_25, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_26_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_26, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_27_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_27, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_28_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_28, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_29_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_29, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_30_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_30, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_31_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_31, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_32_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_32, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_33_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_33, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_34_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_34, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_35_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_35, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_36_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_36, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_37_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_37, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_38_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_38, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_39_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_39, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_40_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_40, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_41_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_41, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_42_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_42, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_43_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_43, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_44_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_44, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_45_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_45, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_46_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_46, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_47_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_47, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_48_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_48, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_49_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_49, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_50_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_50, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_51_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_51, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_51_DiffuseTexID.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::OBJECT_ROCK_51, pLayerTag, TEXT("ObjectRock"), &EditionDesc)))
		return E_FAIL;

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
