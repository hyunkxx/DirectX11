#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"
#include "Character.h"
#include "P_PlayerGirl.h"
#include "M_GAzizi.h"
#include "AcquireSystem.h"

#include "MapObject.h"

#include "Item.h"

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

	CP_PlayerGirl::Init_States(m_pDevice, m_pContext);
	CM_GAzizi::Init_States(m_pDevice, m_pContext);

	if(FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("layer_monster"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_UI(TEXT("layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Tree(TEXT("layer_tree"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Rock(TEXT("layer_rock"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Floor(TEXT("layer_floor"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Stairs(TEXT("layer_stairs"))))
		return E_FAIL;
	

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

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));

	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/Terrain/UVSamplerRatio.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TERRAIN, pLayerTag, L"terrain", &EditionDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::SKY, pLayerTag, L"sky")))
		return E_FAIL;

	_float3 vPos = { 9.f, 30.f, 0.f };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHEST_SIMPLE, pLayerTag, L"chest_simple", &vPos)))
		return E_FAIL;

	vPos = { 9.f, 30.f, 5.f };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHEST_STANDARD, pLayerTag, L"chest_standard", &vPos)))
		return E_FAIL;

	vPos = { 9.f, 30.f, 10.f };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHEST_EXPANDED, pLayerTag, L"chest_expanded", &vPos)))
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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYER_PLAYERGIRL, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::SANDBAG, pLayerTag, TEXT("Sandbag"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MONSTER_GAZIZI, pLayerTag, TEXT("GAzizi"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::SANDBAG, pLayerTag, TEXT("Sandbag"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UI, pLayerTag, TEXT("UI_MainScreen"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MOUSE, pLayerTag, TEXT("UI_Mouse"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UITAPT, pLayerTag, TEXT("UI_TapT"))))
		return E_FAIL;
	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIMONSTER, pLayerTag, TEXT("UI_Monster"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_GameObjectEx(&m_pAcquireSystem, LEVEL_GAMEPLAY, OBJECT::ACQUIRE_SYSTEM, pLayerTag, TEXT("AcquireSystem"))))
		return E_FAIL;

	pGameMode->SetupAcquireSystem(static_cast<CAcquireSystem*>(m_pAcquireSystem));


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Tree(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	
	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_0_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_1_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_1");
		return E_FAIL;
	}
	/*
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_2_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_3_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_4_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_5_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_6_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_7_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_8_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_9_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_10_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_10");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_11_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_12_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_13_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_14_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_15_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_16_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_17_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_18_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_19_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_19");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_20_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_20");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_21_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_22_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_23_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_24_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_25_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_26_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_27_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_27");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_28_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_29_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_29");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Trees/Tree_30_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_30");
		return E_FAIL;
	}
	*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Rock(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_0_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_1_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_2_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_3_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_4_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_5_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_6_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_7_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_8_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_9_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_10_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_10");
		return E_FAIL;
	}

	
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_11_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_12_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_13_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_14_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_15_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_16_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_17_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_18_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_19_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_19");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_20_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_20");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_21_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_22_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_23_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_24_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_25_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_26_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_27_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_27");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_28_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_29_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_29");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_30_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_30");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_31_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_31");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_32_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_32");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_33_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_33");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_34_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_34");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_35_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_35");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_36_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_36");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_37_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_37");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_38_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_38");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_39_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_39");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_40_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_40");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_41_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_41");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_42_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_42");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_43_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_43");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_44_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_44");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_45_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_45");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_46_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_46");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_47_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_47");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_48_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_48");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_49_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_49");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_50_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_50");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_51_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_51");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Rocks/Rock_52_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_52");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Floor(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Floors/Floor_0_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Floors/Floor_1_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Floors/Floor_2_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Floors/Floor_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Floors/Floor_4_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_4");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Stairs(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_2_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_4_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_5_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_6_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_7_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_8_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/MapObject/Stairs/Stairs_9_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_9");
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

	CP_PlayerGirl::Release_States();
	CM_GAzizi::Release_States();
	
}
