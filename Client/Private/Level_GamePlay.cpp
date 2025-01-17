#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"
#include "Character.h"

#include "P_PlayerGirl.h"
#include "P_Yangyang.h"
#include "P_Chixia.h"

#include "M_GAzizi.h"
#include "M_Anjin.h"
#include "M_AWukaka.h"
#include "M_FHuxiuxiu.h"

#include "AcquireSystem.h"
#include "TerminalUI.h"

#include "UICharacter.h"
#include "MapObject.h"

#include "Item.h"
#include "Layer.h"
#include "LightManager.h"
#include "Level_Loading.h"

#include "Boom_Rock.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->SetFog(true);

	_float4 vFogColor = _float4(0.9f, 0.9f, 0.9f, 1.f);
	pGameInstance->SetFogColor(vFogColor);
	pGameInstance->SetLUT(CRenderer::LUT_FUJI);

	// 몬스터들 상태 초기화 해놓기
	CP_PlayerGirl::Init_States(m_pDevice, m_pContext);
	CP_Yangyang::Init_States(m_pDevice, m_pContext);
	CP_Chixia::Init_States(m_pDevice, m_pContext);
	CM_GAzizi::Init_States(m_pDevice, m_pContext);
	CM_Anjin::Init_States(m_pDevice, m_pContext);
	CM_AWukaka::Init_States(m_pDevice, m_pContext);
	CM_FHuxiuxiu::Init_States(m_pDevice, m_pContext);

#pragma region Ready_Layer
	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("layer_monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
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

	if (FAILED(Ready_Interaction_Object(TEXT("layer_Interaction_Object"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trigger(TEXT("layer_trigger"))))
		return E_FAIL;
#pragma endregion

	CLayer* pEchoLayer = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("EchoInstance"));

	if (nullptr != pEchoLayer)
	{
		for (auto& pMonster : pEchoLayer->m_GameObjects)
			static_cast<CCharacter*>(pMonster.second)->Reload_Components();
	}

	pGameInstance->StartFade(CRenderSetting::FADE_IN, 4.f);

	//pGameInstance->SetVolume(SOUND_TYPE::SOUND_BGM, 0.2f);
	//pGameInstance->PlaySoundEx(L"Base_BGM.mp3", SOUND_CHANNEL::SOUND_BGM, VOLUME_BGM);
	//pGameInstance->PlaySoundEx(L"Play_AMB_Common_Insect.wem.wav", SOUND_CHANNEL::SOUND_AMBIENCE1, VOLUME_BGM);

	pGM->ResetStaticShadowBake();

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

	if (nullptr == pGameInstance || nullptr == pGameMode)
		return;

	/*
	// 전투 상태인데
	if (true == pGameMode->Is_Battle())
	{
		// 계속 전투중 이라면 거리 벗어남 시간 누적 초기화. + 전투 지속 시간 체크
		if (true == pGameMode->Is_Combat())
		{
			m_Battle_TimeAcc = 0.0;

			m_Combat_TimeAcc += TimeDelta;

			// 전투 지속이 갱신되면 전투 지속 관련 초기화
			if (true == pGameMode->Is_RenewalCombat())
			{
				m_Combat_TimeAcc = 0.0;
				pGameMode->Reset_RenewalCombat();
			}
		}
		// 제한 전투 지속 시간 이 지나면
		if (m_Combat_Time <= m_Combat_TimeAcc)
		{
			// 전투 지속 관련 초기화
			m_Combat_TimeAcc = 0.0;
			pGameMode->Reset_Combat();
		}

		// 전투 거리 안에 없을경우. -> 시간체크
		if (false == pGameMode->IsIn_BattleRange())
		{
			m_Battle_TimeAcc += TimeDelta;

			// 전투 상태일때 거리가 멀어지면. 일정시간 경과시 비전투 상태로 세팅.
			if (m_Battle_Time <= m_Battle_TimeAcc)
			{
				m_Battle_TimeAcc = 0.0;
				pGameMode->Reset_Battle();

				pGameMode->SetUp_ChangeDelay();
			}
		}
	}

	if (true == pGameMode->Is_ChangeDelay())
	{
		if (true == pGameInstance->BGMSmoothOff(TimeDelta))
		{
			if (true == pGameMode->Is_Battle())
			{
				//pGameInstance->BGMSmoothOn(TimeDelta);
				pGameInstance->StopSound(SOUND_CHANNEL::SOUND_BGM);
				pGameInstance->StopSound(SOUND_CHANNEL::SOUND_AMBIENCE1);
				pGameInstance->PlaySoundEx(L"Battle_BGM_0.mp3", SOUND_CHANNEL::SOUND_BGM, VOLUME_BGM);
			}
			else
			{
				//pGameInstance->BGMSmoothOn(TimeDelta);
				pGameInstance->StopSound(SOUND_CHANNEL::SOUND_BGM);
				pGameInstance->StopSound(SOUND_CHANNEL::SOUND_AMBIENCE1);
				pGameInstance->PlaySoundEx(L"Base_BGM.mp3", SOUND_CHANNEL::SOUND_BGM, VOLUME_BGM);
				pGameInstance->PlaySoundEx(L"Play_AMB_Common_Insect.wem.wav", SOUND_CHANNEL::SOUND_AMBIENCE1, VOLUME_BGM);
			}

			pGameMode->Reset_ChangeDelay();
		}
	}
	else
	{
		pGameInstance->BGMSmoothOn(TimeDelta);
	}
	*/

#pragma region  Key_Input
	////임시 그래픽 세팅 추후에 시스템 UI만들면서 넣을것
	//static _uint iShadowLevel = 0;
	//if (pGameInstance->InputKey(DIK_0) == KEY_STATE::TAP)
	//{
	//	iShadowLevel++;
	//	if (iShadowLevel > 2)
	//		iShadowLevel = 0;

	//	pGameInstance->SetShadowLevel((CRenderSetting::SHADOW_LEVEL)iShadowLevel);
	//}

	//if (pGameInstance->InputKey(DIK_PAUSE) == KEY_STATE::TAP)
	//{
	//	pGameInstance->DebugTargetToggle();
	//}

	//if (pGameInstance->InputKey(DIK_9) == KEY_STATE::TAP)
	//	pGameInstance->OutlineToggle();

	//if (pGameInstance->InputKey(DIK_8) == KEY_STATE::TAP)
	//	pGameInstance->SSAOToggle();

	//static _uint iIndexLUT = CRenderer::LUT_DEFAULT;
	//if (pGameInstance->InputKey(DIK_7) == KEY_STATE::TAP)
	//{
	//	iIndexLUT++;
	//	if (iIndexLUT >= CRenderer::LUT_DEFAULT + 1)
	//		iIndexLUT = 0;

	//	pGameInstance->SetLUT((CRenderer::LUT)iIndexLUT);
	//}

	//if (pGameInstance->InputKey(DIK_LCONTROL) == KEY_STATE::HOLD &&
	//	pGameInstance->InputKey(DIK_6) == KEY_STATE::TAP)
	//{
	//	pGameInstance->StartBlackWhite(0.05f);
	//}
	//else if (pGameInstance->InputKey(DIK_6) == KEY_STATE::TAP)
	//{
	//	pGameInstance->StartBlackWhite(1.f);
	//}

	//if (pGameInstance->InputKey(DIK_LCONTROL) == KEY_STATE::HOLD &&
	//	pGameInstance->InputKey(DIK_5) == KEY_STATE::TAP)
	//{
	//	pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 0.5f);
	//}
	//else if (pGameInstance->InputKey(DIK_5) == KEY_STATE::TAP)
	//{
	//	pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 2.f);
	//}

	if (pGameInstance->InputKey(DIK_L) == KEY_STATE::TAP)
		pGameInstance->TimeSlowDown(1.f, 0.1f);

	if (pGameInstance->InputKey(DIK_K) == KEY_STATE::TAP)
		pGameInstance->TimeSlowDown(0.5f, 0.1f);
#pragma endregion

	if (true == pGameMode->Is_ReserveLevel())
	{
			pGameInstance->StopAllSound();

		//if (true == pGameInstance->BGMSmoothOff(TimeDelta))
		//{
			pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, pGameMode->Get_ReserveLevel()));
			pGameMode->Reset_ReserveLevel();
		//}
	}

#pragma region LEVEL_MOVE
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F2))
	{
		pGameInstance->StopAllSound();
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CITY));
	}
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F3))
	{
		pGameInstance->StopAllSound();
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FOREST));
	}
	if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F4))
	{
		pGameInstance->StopAllSound();
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CROWN));
	}
#pragma endregion LEVEL_MOVE

}

void CLevel_GamePlay::RenderLevelUI()
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

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

#pragma region BackGround
	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));

	EditionDesc.pEditionFilePath = TEXT("Terrain/UVSamplerRatio.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TERRAIN, pLayerTag, L"terrain", &EditionDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::SKY, pLayerTag, L"sky")))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

#pragma region  Camera
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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYER_CAMERA, pLayerTag, L"player_camera", &PCamDesc)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
#pragma region  Player
	/*int j = 0;
	_tchar szTag[MAX_TAG];
	wsprintf(szTag, L"Texture%d", j);
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHARACTER, pLayerTag, szTag, &j)))
	return E_FAIL;*/

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYER_PLAYERGIRL, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYER_YANGYANG, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::PLAYER_CHIXIA, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::SANDBAG, pLayerTag, TEXT("Sandbag"))))
	//	return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	SPAWN_POINT			SpawnPoint = {};

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	CGameObject* pChar = nullptr;

	// [ 돌판 1 ]
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 145.898f, 25.767f, 183.038f };
	SpawnPoint.iCellIndex = 1340;

	if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_GAMEPLAY, OBJECT::MONSTER_GAZIZI, pLayerTag, TEXT("Gazizi"))))
		return E_FAIL;

	static_cast<CCharacter*>(pChar)->Set_InitPos(XMLoadFloat3(&SpawnPoint.vP), SpawnPoint.iCellIndex);
	static_cast<CM_GAzizi*>(pChar)->SetUp_Hp(2000.0f);

	/*ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 130.752f, 25.751f, 186.065f };
	SpawnPoint.iCellIndex = 1460;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_AWUKAKA, pLayerTag, SpawnPoint)))
		return E_FAIL;*/

	// [ 돌판 2 ]
	/*ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 112.537f, 27.586f, 221.782f };
	SpawnPoint.iCellIndex = 1681;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_AWUKAKA, pLayerTag, SpawnPoint)))
		return E_FAIL;
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 104.934f, 27.727f, 228.272f };
	SpawnPoint.iCellIndex = 1668;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_FHUXIUXIU, pLayerTag, SpawnPoint)))
		return E_FAIL;*/

	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 108.511f, 27.577f, 212.447f };
	SpawnPoint.iCellIndex = 1584;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_AWUKAKA, pLayerTag, SpawnPoint)))
		return E_FAIL;

	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 110.679f, 27.628f, 223.045f };
	SpawnPoint.iCellIndex = 1682;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_FHUXIUXIU, pLayerTag, SpawnPoint)))
		return E_FAIL;

	// [ 돌판 3 ] 
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 142.984f, 29.651f, 243.750f };
	SpawnPoint.iCellIndex = 1926;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_GAZIZI, pLayerTag, SpawnPoint)))
		return E_FAIL;
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 133.843f, 29.642f, 246.874f };
	SpawnPoint.iCellIndex = 1900;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_FHUXIUXIU, pLayerTag, SpawnPoint)))
		return E_FAIL;

	// [ 돌판 5 ] (용)
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 186.600f, 32.838f, 242.434f };
	SpawnPoint.iCellIndex = 2214;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_ANJIN, pLayerTag, SpawnPoint)))
		return E_FAIL;

	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 197.877f, 32.839f, 232.986f };
	SpawnPoint.iCellIndex = 2220;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_ANJIN, pLayerTag, SpawnPoint)))
		return E_FAIL;

	// [ 랏 동굴 ]
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));
	SpawnPoint.vP = { 328.355f, 75.063f, 336.669f };
	SpawnPoint.iCellIndex = 4049;
	if (FAILED(Add_StreetMonster(OBJECT::MONSTER_ANJIN, pLayerTag, SpawnPoint)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

#pragma region UI
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIWeapon, pLayerTag, TEXT("UIWeapon"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UICharacter, pLayerTag, TEXT("UI_Character"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UI, pLayerTag, TEXT("UI_MainScreen"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MOUSE, pLayerTag, TEXT("UI_Mouse"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UITAPT, pLayerTag, TEXT("UI_TapT"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIMINIMAP, pLayerTag, TEXT("UI_Minimap"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UITIP, pLayerTag, TEXT("UI_Tip"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIMERCHANTMEN, pLayerTag, TEXT("UI_MerchantMen"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UISOUVI, pLayerTag, TEXT("UI_Souvenir"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIPANHUA, pLayerTag, TEXT("UI_Panhua"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UICOOKING, pLayerTag, TEXT("UI_Cooking"))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Tree(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	
	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region Map_Object
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_0_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_1_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_1");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_2_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_3_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_4_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_5_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_6_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_7_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_8_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_9_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_10_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_10");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_11_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_12_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_13_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_14_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_15_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_16_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_17_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_18_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_19_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_19");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_20_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_20");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_21_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_22_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_23_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_24_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_25_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_26_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_27_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_27");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_28_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_29_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_29");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_30_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_30");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Trees/Tree_31_EditionColor.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Tree"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_TREE_31");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Rock(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	
#pragma region Map_Obejct_Rock
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_0_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_5;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_1_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_6;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_2_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_7;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_3_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_8;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_4_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_9;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_5_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_10;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_6_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_7_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_12;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_8_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_14;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_9_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_14;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_10_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_15;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_10");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_11_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_16;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_12_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_17;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_13_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_22;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_14_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_23;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_15_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_24;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_16_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_25;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_17_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_26;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_18_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_19_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_28;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_19");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_20_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_29;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_20");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_21_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_28;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_22_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_28;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_23_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_24_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_25_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_32;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_26_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_35;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_27_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_27");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_28_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_29_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_29");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_30_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_30");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_31_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_31");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_32_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_32");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_33_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 30.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_33");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_34_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_42;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_34");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_35_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_43;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_35");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_36_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_NONE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_36");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_37_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_45;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_37");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_38_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_50;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_38");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_39_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_52;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_39");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_40_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_55;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_40");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_41_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_66;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_41");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_42_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_67;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_42");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_43_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_68;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_43");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_44_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_70;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_44");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_45_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_71;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_45");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_46_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_72;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_46");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_47_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_76;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_47");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_48_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_77;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_48");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_49_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_78;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_49");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_50_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_80;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_50");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_51_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_82;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_51");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Rocks/Rock_52_DiffuseTexID.data");
	EditionDesc.fCullingRatio = 50.0f;
	EditionDesc.iMaskTex_ID = STATIC_IMAGE::ROCK_MASK_83;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Rock"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_ROCK_52");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Floor(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region Floor
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_0_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_1_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_2_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Floors/Floor_4_Edition.data");;
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Floor"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_FLOOR_4");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Stairs(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region MapObject_Stairs
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_2_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_3_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_4_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_5_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_6_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_7_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_8_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Stairs/Stairs_9_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Stairs"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STAIRS_9");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Grass(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region MapObject_Grass

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_6_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_7_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_8_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_9_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_9");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_10_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_10");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_11_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_11");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_12_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_12");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_13_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_13");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_14_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_14");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_15_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_15");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Grass/Grass_16_SubEditionMask.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Grass"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_GRASS_16");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Vin(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	
#pragma region MapObject_Vin
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_6_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_7_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_8_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_9_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_9");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_10_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_10");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Vin/Vin_11_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Vin"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VIN_11");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Veg(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region MapObject_Veg

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_6_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_7_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_8_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_9_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_9");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_10_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_10");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_11_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_11");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_12_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_12");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_13_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_13");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_14_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_14");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_15_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_15");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_16_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_16");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_17_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_17");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_18_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_18");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_19_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_19");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_20_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_20");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_21_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_21");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_22_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_22");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_23_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_23");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_24_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_24");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_25_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_25");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_26_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_26");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_27_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_27");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_28_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_28");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_29_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_29");
		return E_FAIL;
	}


	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_30_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_30");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_31_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_31");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Veg/Veg_32_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Veg"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_VEG_32");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Shr(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region MapObject_Shr
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_0");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_3");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_4_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_4");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_5_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_5");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_6_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_6");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_7_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_7");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_8_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_8");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Shr/Shr_9_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Shr"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_SHR_9");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Structure(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region MapObject_Structure
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STRUCTURE_0");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STRUCTURE_1");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STRUCTURE_2");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Structure/Structure_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Structure"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STRUCTURE_3");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Pil(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

#pragma region MapObject_Pil
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_0_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_PIL_0");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_1_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_PIL_1");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_2_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_PIL_2");
		return E_FAIL;
	}
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Pil/Pil_3_Edition.data");
	EditionDesc.fCullingRatio = 20.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Pil"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_PIL_3");
		return E_FAIL;
	}

#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject_Statue(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

#pragma region MapObject_Statue
	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Statue/Statue_0_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Statue"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STATUE_0");
		return E_FAIL;
	}

	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	EditionDesc.pEditionFilePath = TEXT("MapObject/Statue/Statue_1_Edition.data");
	EditionDesc.fCullingRatio = 30.0f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::MAP_OBJECT, pLayerTag, TEXT("MapObject_Statue"), &EditionDesc)))
	{
		MSG_BOX("Failed to AddGameObejct In Level_GamePlay : SIMD_STATUE_1");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Interaction_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

#pragma region Interaction_Object
	_float3  vPos = { 135.5f, 30.75f, 211.f };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::INVISIBLE_CHEST_SIMPLE, pLayerTag, L"Invisivle_chest_simple_01", &vPos)))
		return E_FAIL;

	vPos = { 368.f, 71.25f, 286.f };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::INVISIBLE_CHEST_SIMPLE, pLayerTag, L"Invisivle_chest_Simple_02", &vPos)))
		return E_FAIL;

	vPos = { 212.91f,  42.80f,  259.54f, };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::INVISIBLE_CHEST_EXPANDED, pLayerTag, L"chest_expanded", &vPos)))
		return E_FAIL;

	vPos = { 259.55f , 38.30f , 227.42f };
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CHEST_EXPANDED, pLayerTag, L"chest_standard", &vPos)))
		return E_FAIL;

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_float4x4 vWorldMatrix;
	XMStoreFloat4x4(&vWorldMatrix, XMMatrixIdentity());
	vWorldMatrix._41 = 238.f;
	vWorldMatrix._42 = 39.f;
	vWorldMatrix._43 = 242.f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::INTERACTION_OBJECT_BOMBER, pLayerTag, L"Object_Bomber", &vWorldMatrix)))
		return E_FAIL;

	CBoom_Rock::ROCK_STATE Rock_State;
	ZeroMemory(&Rock_State, sizeof(CBoom_Rock::ROCK_STATE));

	Rock_State.Rick_ID = (_int)(SMODEL::INTERACTION_ROCK);
	Rock_State.vExtents = _float3(17.f, 25.f, 8.f);
	Rock_State.iPass = 11;

	XMStoreFloat4x4(&vWorldMatrix, XMMatrixIdentity());
	_matrix matScale = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	_matrix RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(-60.f));
	vWorldMatrix._41 = 260.f;
	vWorldMatrix._42 = 38.f;
	vWorldMatrix._43 = 227.f;

	XMStoreFloat4x4(&Rock_State.vWorldMatrix, matScale * RotationMatrix * XMLoadFloat4x4(&vWorldMatrix));

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::INTERACTION_OBJECT_ROCK, pLayerTag, L"Object_Rock", &Rock_State)))
		return E_FAIL;
#pragma endregion

#pragma region PosList
	vector<_float4>* pPosList = new vector<_float4>;

	_float4 vPos1 = _float4(59.f, 5.f, 38.f, 1.f);

	_float4 Pos1 = _float4(60.39f, 4.91f, 35.86f, 0.f);
	_float4 Pos2 = _float4(79.51f, 5.32f, 35.41f, 0.f);
	_float4 Pos3 = _float4(80.13f, 6.43f, 41.46f, 0.f);

	_float4 vPos2 = _float4(95.f, 6.5f, 54.f, 1.f);

	_float4 Pos4 = _float4(98.24f, 7.04f, 61.94f, 0.f);
	_float4 Pos5 = _float4(98.24f, 17.26f, 61.94f, 0.f);
	_float4 Pos6 = _float4(100.78f, 17.26f, 66.38f, 0.f);

	_float4 vPos3 = _float4(102.f, 16.5f, 69.f, 1.f);

	_float4 Pos7 = _float4(109.46f, 16.85f, 90.43f, 0.f);
	_float4 Pos8 = _float4(120.70f, 17.12f, 94.78f, 0.f);
	_float4 Pos9 = _float4(120.70f, 20.68f, 94.78f, 0.f);

	_float4 vPos4 = _float4(126.f, 20.5f, 96.f, 1.f);

	_float4 Pos10 = _float4(154.38f, 20.68f, 99.41f, 0.f);
	_float4 Pos11 = _float4(154.38f, 25.79f, 99.41f, 0.f);

	_float4 vPos5 = _float4(162.13f, 25.79f, 104.14f, 1.f);

	_float4 Pos12 = _float4(172.77f, 25.79f, 117.25f, 0.f);

	_float4 vPos6 = _float4(175.f, 24.5f, 135.f, 0.f);

	_float4 Pos13 = _float4(172.95f, 25.42f, 150.47f, 0.f);
	_float4 Pos14 = _float4(154.13f, 25.55f, 168.53f, 0.f);
	_float4 Pos15 = _float4(149.71f, 27.24f, 172.51f, 0.f);
	_float4 Pos16 = _float4(135.76f, 27.25f, 181.42f, 0.f);
	_float4 Pos17 = _float4(122.21f, 27.37f, 195.94f, 0.f);
	_float4 Pos18 = _float4(118.27f, 29.03f, 199.81f, 0.f);

	_float4 vPos7 = _float4(110.f, 29.f, 211.f, 1.f);

	_float4 Pos19 = _float4(129.60f, 29.07f, 222.26f, 0.f);
	_float4 Pos20 = _float4(134.78f, 31.11f, 226.97f, 0.f);
	_float4 Pos21 = _float4(145.02f, 31.20f, 251.89f, 0.f);
	_float4 Pos22 = _float4(148.85f, 32.79f, 255.92f, 0.f);

	_float4 vPos8 = _float4(155.f, 33.f, 260.f, 1.f);

	_float4 Pos23 = _float4(168.63f, 32.59f, 254.70f, 0.f);
	_float4 Pos24 = _float4(174.30f, 34.37f, 250.32f, 0.f);

	_float4 vPos9 = _float4(215.f, 35.f, 228.f, 1.f);

	_float4 Pos25 = _float4(226.37f, 39.85f, 221.41f, 0.f);

	_float4 vPos10 = _float4(233.f, 39.5f, 218.f, 1.f);
	_float4 vPos11 = _float4(240.f, 39.5f, 241.f, 1.f);
	_float4 vPos12 = _float4(257.f, 39.5f, 228.f, 1.f);

	_float4 Pos26 = _float4(241.52f, 39.69f, 236.27f, 0.f);

	_float4 vPos13 = _float4(237.f, 39.5f, 209.f, 1.f);

	_float4 Pos27 = _float4(241.91f, 41.75f, 202.92f, 0.f);
	_float4 Pos28 = _float4(250.55f, 44.63f, 192.59f, 0.f);
	_float4 Pos29 = _float4(255.86f, 45.40f, 188.37f, 0.f);
	_float4 Pos30 = _float4(264.65f, 46.07f, 183.20f, 0.f);
	_float4 Pos31 = _float4(270.21f, 48.03f, 180.00f, 0.f);

	_float4 vPos14 = _float4(280.f, 49.f, 178.f, 1.f);

	_float4 Pos32 = _float4(299.26f, 54.08f, 182.30f, 0.f);
	_float4 Pos33 = _float4(299.26f, 54.58f, 182.30f, 0.f);
	_float4 Pos34 = _float4(323.22f, 60.58f, 197.45f, 0.f);

	_float4 vPos15 = _float4(338.f, 64.2f, 209.f, 1.f);

	_float4 Pos35 = _float4(340.56f, 65.52f, 227.78f, 0.f);
	_float4 Pos38 = _float4(343.56f, 66.02f, 227.78f, 0.f);

	_float4 vPos16 = _float4(345.f, 73.f, 282.f, 1.f);


	_float4 Pos36 = _float4(336.85f, 73.81f, 295.63f, 0.f);
	_float4 Pos37 = _float4(330.17f, 74.57f, 327.33f, 0.f);

	_float4 vPos17 = _float4(332.f, 76.5f, 343.f, 1.f);
	_float4 vPos18 = _float4(354.f, 77.1f, 372.f, 1.f);

	pPosList->push_back(vPos1);

	pPosList->push_back(Pos1);
	pPosList->push_back(Pos2);
	pPosList->push_back(Pos3);

	pPosList->push_back(vPos2);

	pPosList->push_back(Pos4);
	pPosList->push_back(Pos5);
	pPosList->push_back(Pos6);

	pPosList->push_back(vPos3);

	pPosList->push_back(Pos7);
	pPosList->push_back(Pos8);
	pPosList->push_back(Pos9);

	pPosList->push_back(vPos4);

	pPosList->push_back(Pos10);
	pPosList->push_back(Pos11);

	pPosList->push_back(vPos5);

	pPosList->push_back(Pos12);

	pPosList->push_back(vPos6);

	pPosList->push_back(Pos13);
	pPosList->push_back(Pos14);
	pPosList->push_back(Pos15);
	pPosList->push_back(Pos16);
	pPosList->push_back(Pos17);
	pPosList->push_back(Pos18);

	pPosList->push_back(vPos7);

	pPosList->push_back(Pos19);
	pPosList->push_back(Pos20);
	pPosList->push_back(Pos21);
	pPosList->push_back(Pos22);

	pPosList->push_back(vPos8);

	pPosList->push_back(Pos23);
	pPosList->push_back(Pos24);

	pPosList->push_back(vPos9);

	pPosList->push_back(Pos25);

	pPosList->push_back(vPos10);
	pPosList->push_back(vPos11);
	pPosList->push_back(vPos12);

	pPosList->push_back(Pos26);

	pPosList->push_back(vPos13);

	pPosList->push_back(Pos27);
	pPosList->push_back(Pos28);
	pPosList->push_back(Pos29);
	pPosList->push_back(Pos30);
	pPosList->push_back(Pos31);

	pPosList->push_back(vPos14);

	pPosList->push_back(Pos32);
	pPosList->push_back(Pos33);
	pPosList->push_back(Pos34);

	pPosList->push_back(vPos15);

	pPosList->push_back(Pos35);
	pPosList->push_back(Pos38);

	pPosList->push_back(vPos16);

	pPosList->push_back(Pos36);
	pPosList->push_back(Pos37);

	pPosList->push_back(vPos17);
	pPosList->push_back(vPos18);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::INTERACTION_GUIDE_SPIRIT, pLayerTag, L"Object_Guide_Spirit", pPosList)))
		return E_FAIL;

	Safe_Delete(pPosList);
#pragma endregion

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Trigger(const _tchar * pLayerTag)
{
	if (FAILED(Load_TriggerData(TEXT("../../Data/GamePlay/Trigger/Potal_City.data"), TEXT("Trigger_Poatal_City"), pLayerTag)))
	{
		MSG_BOX("Trigger_Poatal_City");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_TriggerData(const _tchar * pDataFilePath, const _tchar * pObjectTag, const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	HANDLE		hFile = CreateFile(pDataFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in LEVEL_GAMEPLAY : Trigger");
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


	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::TRIGGER,
		pLayerTag, szObjectTag, &TriggerDesc)))
	{
		MSG_BOX("Failed to Load & AddGameObejct In LEVEL_GAMEPLAY : Trigger");
		return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_GamePlay::Add_StreetMonster(_uint iMonsterNum, const _tchar * pLayerTag, SPAWN_POINT SpawnPoint)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	CGameObject* pChar = nullptr;

	switch (iMonsterNum)
	{
	case OBJECT::MONSTER_GAZIZI:
		if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_GAMEPLAY, OBJECT::MONSTER_GAZIZI, pLayerTag, TEXT("Gazizi"))))
			return E_FAIL;
		break;
	case OBJECT::MONSTER_AWUKAKA:
		if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_GAMEPLAY, OBJECT::MONSTER_AWUKAKA, pLayerTag, TEXT("Awukaka"))))
			return E_FAIL;
		break;
	case OBJECT::MONSTER_FHUXIUXIU:
		if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_GAMEPLAY, OBJECT::MONSTER_FHUXIUXIU, pLayerTag, TEXT("Fhuxiuxiu"))))
			return E_FAIL;
		break;
	case OBJECT::MONSTER_ANJIN:
		if (FAILED(pGameInstance->Add_GameObjectEx(&pChar, LEVEL_GAMEPLAY, OBJECT::MONSTER_ANJIN, pLayerTag, TEXT("Anjin"))))
			return E_FAIL;
		break;

	default:
		return S_OK;
		break;
	}

	static_cast<CCharacter*>(pChar)->Set_InitPos(XMLoadFloat3(&SpawnPoint.vP), SpawnPoint.iCellIndex);

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
	CP_Yangyang::Release_States();
	CP_Chixia::Release_States();
	CM_GAzizi::Release_States();
	CM_Anjin::Release_States();
	CM_AWukaka::Release_States();
	CM_FHuxiuxiu::Release_States();
}
