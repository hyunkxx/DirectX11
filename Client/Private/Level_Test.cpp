#include "stdafx.h"
#include "..\Public\Level_Test.h"

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
#include "M_AWukaka.h"
#include "M_FHuxiuxiu.h"
#include "M_Anjin.h"
#include "M_Huojin.h"
#include "M_Binglie.h"
#include "M_Fenglie.h"
#include "M_Leilie.h"
#include "M_Qunjing.h"
#include "M_Crownless_P1.h"
#include "M_Crownless_P2.h"
#include "M_Crownless_P3.h"
#include "PlayerState.h"

#include "AcquireSystem.h"
#include "Sandbag.h"

#include "MapObject.h"

#include "Item.h"
#include "Layer.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Test::Initialize()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 몬스터들 상태 초기화 해놓기
	CP_PlayerGirl::Init_States(m_pDevice, m_pContext);
	CP_Yangyang::Init_States(m_pDevice, m_pContext);
	CP_Chixia::Init_States(m_pDevice, m_pContext);
	CSandbag::Init_States(m_pDevice, m_pContext);
	CM_FHuxiuxiu::Init_States(m_pDevice, m_pContext);
	CM_Anjin::Init_States(m_pDevice, m_pContext);
	CM_Huojin::Init_States(m_pDevice, m_pContext);
	CM_Binglie ::Init_States(m_pDevice, m_pContext);
	CM_Fenglie::Init_States(m_pDevice, m_pContext);
	CM_Leilie::Init_States(m_pDevice, m_pContext);
	CM_Qunjing::Init_States(m_pDevice, m_pContext);
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

	//if (FAILED(Ready_Layer_MapObject_Tree(TEXT("layer_tree"))))
	//return E_FAIL;

	CLayer* pEchoLayer = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("EchoInstance"));

	if (nullptr != pEchoLayer)
	{
		for (auto& pMonster : pEchoLayer->m_GameObjects)
			static_cast<CCharacter*>(pMonster.second)->Reload_Components();
	}


	pGameInstance->StartFade(CRenderSetting::FADE_IN, 4.f);
	pGameInstance->SetVolume(SOUND_TYPE::SOUND_BGM, 0.2f);
	//pGameInstance->PlaySoundEx(L"Base_BGM.mp3", SOUND_CHANNEL::BGM, VOLUME_BGM);

	return S_OK;
}

void CLevel_Test::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance();
	pAppManager->SetTitle(L"LEVEL_TEST");
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



}

void CLevel_Test::RenderLevelUI()
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

HRESULT CLevel_Test::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	//SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	//ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
	//EditionDesc.pEditionFilePath = TEXT("../../Data/GamePlay/Terrain/UVSamplerRatio.data");
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::TERRAIN, pLayerTag, L"terrain", &EditionDesc)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::FLOOR, pLayerTag, L"floor")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::SKY, pLayerTag, L"sky")))
		return E_FAIL;

	//_float3 vPos = { 32.f, 1.828f, 21.f };
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::CHEST_SIMPLE, pLayerTag, L"chest_simple", &vPos)))
	//	return E_FAIL;

	//vPos = { 28.f, 1.7f, 25.f };
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::CHEST_STANDARD, pLayerTag, L"chest_standard", &vPos)))
	//	return E_FAIL;

	//vPos = { 29.f, 1.83f, 31.f };
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::CHEST_EXPANDED, pLayerTag, L"chest_expanded", &vPos)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _tchar* pLayerTag)
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

	//PlayerCamera Setting
	CPlayerCamera::PLAYERCAM_DESC PCamDesc;
	ZeroMemory(&PCamDesc, sizeof CPlayerCamera::PLAYERCAM_DESC);
	PCamDesc.CamDesc = CameraDesc;
	// PCamDesc.pPlayerTransform  >> Start에서 채움
	// PCamDesc.vDir Start에서 채움
	PCamDesc.fDistance = 4.f;
	PCamDesc.fXAngle = 20.f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::PLAYER_CAMERA, pLayerTag, L"player_camera", &PCamDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CGameObject* pCharacter = nullptr;

	// 한 명만 테스트 할 경우 슬롯을 미리 스왑해 놓아야 함
	//static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"))->Set_ActiveCharacter(CPlayerState::CHARACTER_CHIXIA);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::PLAYER_PLAYERGIRL, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::PLAYER_YANGYANG, pLayerTag, TEXT("Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::PLAYER_CHIXIA, pLayerTag, TEXT("Player"))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::SANDBAG, pLayerTag, TEXT("Sandbag"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_FHUXIUXIU, pLayerTag, TEXT("FHuxiuxiu"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_ANJIN, pLayerTag, TEXT("Anjin"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_BINGLIE, pLayerTag, TEXT("Binglie"))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_FENGLIE, pLayerTag, TEXT("Fenglie"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_LEILIE, pLayerTag, TEXT("Leilie"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_QUNJING, pLayerTag, TEXT("Qunjing"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_HUOJIN, pLayerTag, TEXT("Huojin"))))
	//	return E_FAIL;



	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_CROWNLESS_P1, pLayerTag, TEXT("Crownless_P1"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_CROWNLESS_P2, pLayerTag, TEXT("Crownless_P2"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::MONSTER_CROWNLESS_P3, pLayerTag, TEXT("Crownless_P3"))))
	//	return E_FAIL;


	return S_OK;
}


HRESULT CLevel_Test::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, OBJECT::UI, pLayerTag, TEXT("UI_MainScreen"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectEx(&m_pAcquireSystem, LEVEL_TEST, OBJECT::ACQUIRE_SYSTEM, pLayerTag, TEXT("AcquireSystem"))))
		return E_FAIL;

	pGameMode->SetupAcquireSystem(static_cast<CAcquireSystem*>(m_pAcquireSystem));


	return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_Test";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();

	CP_PlayerGirl::Release_States();
	CP_Yangyang::Release_States();
	CP_Chixia::Release_States();
	CSandbag::Release_States();
	CM_FHuxiuxiu::Release_States();
	CM_Anjin::Release_States();
	CM_Huojin::Release_States();
	CM_Binglie::Release_States();
	CM_Fenglie::Release_States();
	CM_Leilie::Release_States();
	CM_Qunjing::Release_States();
	CM_Crownless_P1::Release_States();
	CM_Crownless_P2::Release_States();
	CM_Crownless_P3::Release_States();
}
