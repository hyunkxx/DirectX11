#include "stdafx.h"
#include "..\Public\Level_Crown.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"
#include "Character.h"
#include "P_PlayerGirl.h"
#include "P_Yangyang.h"
#include "P_Chixia.h"
#include "M_Crownless_P1.h"
#include "M_Crownless_P2.h"
#include "M_Crownless_P3.h"

#include "AcquireSystem.h"
#include "TerminalUI.h"
#include "Layer.h"

#include "UICharacter.h"
#include "MapObject.h"
#include "Trigger.h"

#include "Item.h"

#include "Level_Loading.h"

CLevel_Crown::CLevel_Crown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Crown::Initialize()
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 몬스터들 상태 초기화 해놓기
	CP_PlayerGirl::Init_States(m_pDevice, m_pContext);
	CP_Yangyang::Init_States(m_pDevice, m_pContext);
	CP_Chixia::Init_States(m_pDevice, m_pContext);
	CM_Crownless_P1::Init_States(m_pDevice, m_pContext);
	CM_Crownless_P2::Init_States(m_pDevice, m_pContext);
	CM_Crownless_P3::Init_States(m_pDevice, m_pContext);

	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
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

	if (FAILED(Ready_Layer_MapObject_Grass(TEXT("layer_grass"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Vin(TEXT("layer_vin"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Veg(TEXT("layer_veg"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Shr(TEXT("layer_shr"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Structure(TEXT("layer_structure"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Pil(TEXT("layer_pil"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Statue(TEXT("layer_statue"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_NoiRock(TEXT("layer_noirock"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MapObject_Tof_Grass(TEXT("layer_tof_grass"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trigger(TEXT("layer_trigger"))))
		return E_FAIL;

	CLayer* pEchoLayer = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("EchoInstance"));

	if (nullptr != pEchoLayer)
	{
		for (auto& pMonster : pEchoLayer->m_GameObjects)
			static_cast<CCharacter*>(pMonster.second)->Reload_Components();
	}

	pGameInstance->StartFade(CRenderSetting::FADE_IN, 4.f);
	pGameInstance->SetVolume(SOUND_TYPE::SOUND_BGM, 0.5f);
	pGameInstance->PlaySoundEx(L"Base_BGM.mp3", SOUND_CHANNEL::SOUND_BGM, VOLUME_BGM);

	pGM->ResetStaticShadowBake();

	return S_OK;
}

void CLevel_Crown::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance();
	pAppManager->SetTitle(L"LEVEL_CROWN");
#endif

	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//pGameInstance->BGMSmoothOn(TimeDelta);

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

	if (pGameInstance->InputKey(DIK_9) == KEY_STATE::TAP)
		pGameInstance->OutlineToggle();

	if (pGameInstance->InputKey(DIK_8) == KEY_STATE::TAP)
		pGameInstance->SSAOToggle();

	static _uint iIndexLUT = CRenderer::LUT_DEFAULT;
	if (pGameInstance->InputKey(DIK_7) == KEY_STATE::TAP)
	{
		iIndexLUT++;
		if (iIndexLUT >= CRenderer::LUT_DEFAULT + 1)
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

	if (pGameInstance->InputKey(DIK_L) == KEY_STATE::TAP)
		pGameInstance->TimeSlowDown(1.f, 0.1f);

	if (pGameInstance->InputKey(DIK_K) == KEY_STATE::TAP)
		pGameInstance->TimeSlowDown(0.5f, 0.1f);

	if (true == pGameMode->Is_ReserveLevel())
	{
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, pGameMode->Get_ReserveLevel()));
		pGameMode->Reset_ReserveLevel();
	}

#pragma region LEVEL_MOVE
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F1))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F2))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CITY));
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F3))
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FOREST));
#pragma endregion LEVEL_MOVE

	
}

void CLevel_Crown::RenderLevelUI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	ImGui::Begin("Helper");

	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Frame Rate");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "DynamicCamera");
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

HRESULT CLevel_Crown::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));

	EditionDesc.pEditionFilePath = TEXT("Terrain/UVSamplerRatio.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::TERRAIN, pLayerTag, L"terrain", &EditionDesc)))
		return E_FAIL;

	// SKY_LOBBY SKY
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::SKY, pLayerTag, L"sky")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_Camera(const _tchar* pLayerTag)
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

	_vector vBakeLightPos = XMVectorSet(-80.f, 390.f, -80.f, 1.f);
	_vector vBakeLightAt = XMVectorSet(200.f, 0.f, 200.f, 1.f);

	pGameInstance->BakeShadowLight(vBakeLightPos, vBakeLightAt);

	//PlayerCamera Setting
	CPlayerCamera::PLAYERCAM_DESC PCamDesc;
	ZeroMemory(&PCamDesc, sizeof CPlayerCamera::PLAYERCAM_DESC);
	PCamDesc.CamDesc = CameraDesc;
	// PCamDesc.pPlayerTransform  >> Start에서 채움
	// PCamDesc.vDir Start에서 채움
	PCamDesc.fDistance = 4.f;
	PCamDesc.fXAngle = 20.f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::PLAYER_CAMERA, pLayerTag, L"player_camera", &PCamDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::PLAYER_PLAYERGIRL, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::PLAYER_YANGYANG, pLayerTag, TEXT("Player1"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::PLAYER_CHIXIA, pLayerTag, TEXT("Player2"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(Load_TriggerData(TEXT("../../Data/Crown/Trigger/Spawn_Crown.data"), TEXT("Trigger_Spawn_Crown"),
		TEXT("layer_trigger"), TEXT("../../Data/Crown/Trigger/Spawn_Crown_SpawnPoint.data"))))
	{
		MSG_BOX("Trigger_Spawn_Crown");
		return E_FAIL;
	}

	CGameObject* pChar = nullptr;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_CROWNLESS_P1, pLayerTag, TEXT("Crownless_P1"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_CROWNLESS_P2, pLayerTag, TEXT("Crownless_P2"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_CROWN, OBJECT::MONSTER_CROWNLESS_P3, pLayerTag, TEXT("Crownless_P3"))))
		return E_FAIL;

	//static_cast<CCharacter*>(pChar)->Set_InitPos(XMVectorSet(116.507f, 20.010f, 113.048f, 1.f), 218);
	static_cast<CCharacter*>(pChar)->Set_State(CCharacter::STATE::DISABLE);

	CGameObject*		pGameObject = { nullptr };
	pGameObject = pGameInstance->Find_GameObject(LEVEL_CROWN, TEXT("Trigger_Spawn_Crown"));
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(static_cast<CTrigger*>(pGameObject)->Link_WaveMonster(CTrigger::SPAWN_WAVE::WAVE_1, static_cast<CCharacter*>(pChar))))
		return E_FAIL;


	
	if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_CROWN, OBJECT::PHASECHANGER, pLayerTag, TEXT("PhaseChanger"))))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	pGM->Set_PhaseChanger(static_cast<CPhaseChanger*>(pChar));


	return S_OK;
}


HRESULT CLevel_Crown::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UIWeapon, pLayerTag, TEXT("UIWeapon"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UICharacter, pLayerTag, TEXT("UI_Character"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UI, pLayerTag, TEXT("UI_MainScreen"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MOUSE, pLayerTag, TEXT("UI_Mouse"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UITAPT, pLayerTag, TEXT("UI_TapT"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UIMINIMAP, pLayerTag, TEXT("UI_Minimap"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UITIP, pLayerTag, TEXT("UI_Tip"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::UIMERCHANTMEN, pLayerTag, TEXT("UI_MerchantMen"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Tree(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_0_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_1_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_1");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_2_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_3_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_4_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_5_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_6_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_7_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_8_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_9_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_10_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_10");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_11_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_12_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_13_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_14_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_15_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_16_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_17_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_18_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_19_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_19");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_20_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_20");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_21_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_22_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_23_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_24_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_25_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_26_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_27_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_27");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_28_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_29_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_29");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_30_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_30");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_31_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TREE_31");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Rock(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_0_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_5;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_1_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_6;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_2_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_7;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_3_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_8;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_4_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_9;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_5_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_10;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_6_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_11;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_7_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_12;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_8_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_14;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_9_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_14;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_10_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_15;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_10");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_11_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_16;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_12_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_17;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_13_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_22;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_14_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_23;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_15_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_24;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_16_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_25;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_17_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_26;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_18_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_27;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_19_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_28;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_19");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_20_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_29;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_20");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_21_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_28;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_22_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_28;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_23_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_24_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_25_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_26_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_35;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_27_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_27");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_28_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_29_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_29");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_30_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_30");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_31_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_31");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_32_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_41;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_32");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_33_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 30.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_41_B;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_33");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_34_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_42;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_34");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_35_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_43;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_35");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_36_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_44;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_36");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_37_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_45;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_37");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_38_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_50;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_38");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_39_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_52;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_39");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_40_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_55;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_40");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_41_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_66;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_41");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_42_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_67;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_42");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_43_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_68;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_43");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_44_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_70;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_44");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_45_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_71;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_45");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_46_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_72;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_46");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_47_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_76;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_47");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_48_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_77;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_48");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_49_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_78;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_49");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_50_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_80;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_50");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_51_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_82;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_51");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_52_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_83;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_ROCK_52");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Floor(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_0_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_FLOOR_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_1_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_FLOOR_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_2_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_FLOOR_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_FLOOR_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_4_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_FLOOR_4");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Stairs(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_2_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_4_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_5_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_6_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_7_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_8_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_9_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STAIRS_9");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Grass(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_6_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_7_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_8_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_9_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_9");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_10_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_10");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_11_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_11");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_12_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_12");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_13_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_13");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_14_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_14");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_15_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_15");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_16_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_GRASS_16");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Vin(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_6_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_7_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_8_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_9_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_10_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_10");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_11_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VIN_11");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Veg(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_6_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_7_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_8_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_9_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_9");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_10_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_10");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_11_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_12_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_13_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_14_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_15_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_16_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_17_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_18_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_19_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_19");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_20_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_20");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_21_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_22_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_23_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_24_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_25_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_26_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_27_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_27");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_28_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_29_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_29");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_30_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_30");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_31_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_31");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_32_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_VEG_32");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Shr(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_6_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_7_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_8_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_9_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_SHR_9");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Structure(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STRUCTURE_0");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STRUCTURE_1");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STRUCTURE_2");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STRUCTURE_3");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Pil(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_PIL_0");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_PIL_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_PIL_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_PIL_3");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Statue(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Statue/Statue_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Statue"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STATUE_0");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Statue/Statue_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Statue"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_STATUE_1");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_NoiRock(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_2_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_4_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_5_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_6_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Noi_Rock/Noi_Rock_7_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Noi_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_NOIROCK_7");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_MapObject_Tof_Grass(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_2_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_4_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_5_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_6_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_7_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_8_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_9_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_9");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_10_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_10");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_11_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_12_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_13_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Tof_Grass/Tof_Grass_14_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tof_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In LEVEL_CROWN : SIMD_TOF_GRASS_14");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Crown::Ready_Layer_Trigger(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Crown::Load_TriggerData(const _tchar * pDataFilePath, const _tchar * pObjectTag, const _tchar * pLayerTag, const _tchar* pEditionFilePath)
{
	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	HANDLE		hFile = CreateFile(pDataFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in LEVEL_CROWN : Trigger");
		return E_FAIL;
	}

	_tchar		szObjectTag[MAX_PATH] = { TEXT("") };
	wsprintf(szObjectTag, pObjectTag);

	DWORD		dwByte = 0;

	TRIGGER_DESC		TriggerDesc = {};
	ZeroMemory(&TriggerDesc, sizeof(TRIGGER_DESC));

	ReadFile(hFile, &TriggerDesc.vP, sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &TriggerDesc.vS, sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &TriggerDesc.vA, sizeof(_float3), &dwByte, nullptr);

	ReadFile(hFile, &TriggerDesc.fRange, sizeof(_float), &dwByte, nullptr);

	ReadFile(hFile, &TriggerDesc.iTriggerID, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &TriggerDesc.iTriggerType, sizeof(_uint), &dwByte, nullptr);

	if (nullptr != pEditionFilePath)
		TriggerDesc.pEditionFilePath = pEditionFilePath;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CROWN, OBJECT::TRIGGER,
		pLayerTag, szObjectTag, &TriggerDesc)))
	{
		MSG_BOX("Failed to Load & AddGameObejct In LEVEL_CROWN : Trigger");
		return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

CLevel_Crown* CLevel_Crown::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Crown* pInstance = new CLevel_Crown(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_Crown";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Crown::Free()
{
	__super::Free();

	CP_PlayerGirl::Release_States();
	CP_Yangyang::Release_States();
	CP_Chixia::Release_States();

	CM_Crownless_P1::Release_States();
	CM_Crownless_P2::Release_States();
	CM_Crownless_P3::Release_States();
}
