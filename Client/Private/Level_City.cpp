#include "stdafx.h"
#include "..\Public\Level_City.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"
#include "Character.h"
#include "P_PlayerGirl.h"
#include "M_GAzizi.h"
#include "M_Anjin.h"
#include "M_AWukaka.h"
#include "AcquireSystem.h"
#include "TerminalUI.h"

#include "UICharacter.h"
#include "MapObject.h"

#include "Item.h"

#include "CityObject.h"

CLevel_City::CLevel_City(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_City::Initialize()
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_character"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("layer_monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_CityObject()))
		return E_FAIL;

	pGameInstance->StartFade(CRenderSetting::FADE_IN, 4.f);
	pGameInstance->SetVolume(SOUND_TYPE::SOUND_BGM, 0.5f);
	pGameInstance->PlaySoundEx(L"Base_BGM.mp3", SOUND_CHANNEL::BGM, VOLUME_BGM);

	pGM->ResetStaticShadowBake();

	return S_OK;
}

void CLevel_City::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CAppManager* pAppManager = CAppManager::GetInstance();
	pAppManager->SetTitle(L"LEVEL_CITY");
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

void CLevel_City::RenderLevelUI()
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

HRESULT CLevel_City::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	SMAP_OBJECT_EDITION_DESC		EditionDesc = {};
	ZeroMemory(&EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));

	EditionDesc.pEditionFilePath = TEXT("Terrain/UVSamplerRatio.data");
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CITY, OBJECT::TERRAIN, pLayerTag, L"terrain", &EditionDesc)))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_CITY, OBJECT::SKY, pLayerTag, L"sky")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_City::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance || nullptr == pGM)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_City::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_City::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_City::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_City::Ready_Layer_CityObject()
{
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_0.data"), TEXT("CityObejct_Bui_0_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_1.data"), TEXT("CityObejct_Bui_1_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_2.data"), TEXT("CityObejct_Bui_2_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_3.data"), TEXT("CityObejct_Bui_3_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_4.data"), TEXT("CityObejct_Bui_4_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_5.data"), TEXT("CityObejct_Bui_5_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_6.data"), TEXT("CityObejct_Bui_6_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_7.data"), TEXT("CityObejct_Bui_7_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_8.data"), TEXT("CityObejct_Bui_8_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_9.data"), TEXT("CityObejct_Bui_9_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_10.data"), TEXT("CityObejct_Bui_10_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_11.data"), TEXT("CityObejct_Bui_11_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_12.data"), TEXT("CityObejct_Bui_12_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_13.data"), TEXT("CityObejct_Bui_13_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_14.data"), TEXT("CityObejct_Bui_14_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_15.data"), TEXT("CityObejct_Bui_15_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_16.data"), TEXT("CityObejct_Bui_16_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Bui_17.data"), TEXT("CityObejct_Bui_17_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Ele_0.data"), TEXT("CityObejct_Ele_0_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Rai_0.data"), TEXT("CityObejct_Rai_0_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Gro_0.data"), TEXT("CityObejct_Gro_0_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Sta_0.data"), TEXT("CityObejct_Sta_0_%d"))))
		return E_FAIL;
	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Sta_1.data"), TEXT("CityObejct_Sta_1_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Des_0.data"), TEXT("CityObejct_Des_0_%d"))))
		return E_FAIL;

	if (FAILED(Load_CityObject(TEXT("../../Data/City/CityObject/Cit_Sto_0.data"), TEXT("CityObejct_Sto_0_%d"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_City::Load_CityObject(const _tchar* pDataFilePath, const _tchar* pCityObjectTag)
{
	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	HANDLE		hFile = CreateFile(pDataFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in Level_GamePlay : City_Object");
		return E_FAIL;
	}

	const _tchar*			pLayerTag = { TEXT("") };

	const _tchar*			pCityObejctTag = pCityObjectTag;
	_tchar					szCityObjectTag[MAX_PATH] = { TEXT("") };

	DWORD		dwByte = 0;

	SCITY_OBJECT_DESC		CityObject_Desc = {};

	_uint			iTotalCount = { 0 };

	ReadFile(hFile, &iTotalCount, sizeof(_uint), &dwByte, nullptr);

	if (0 >= iTotalCount)
		return S_OK;

	for (_uint i = 0; i < iTotalCount; ++i)
	{
		ZeroMemory(&CityObject_Desc, sizeof(SCITY_OBJECT_DESC));

		ReadFile(hFile, &CityObject_Desc.PSA.vP, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &CityObject_Desc.PSA.vS, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &CityObject_Desc.PSA.vA, sizeof(_float3), &dwByte, nullptr);

		ReadFile(hFile, &CityObject_Desc.iSMD_ID, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &CityObject_Desc.iType_ID, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &CityObject_Desc.iOption_ID, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &CityObject_Desc.iSelf_ID, sizeof(_uint), &dwByte, nullptr);

		switch (CityObject_Desc.iOption_ID)
		{
		case Client::CCityObject::OP_DECO:
			pLayerTag = TEXT("layer_city_deco");
			break;
		case Client::CCityObject::OP_FLOOR:
			pLayerTag = TEXT("layer_city_floor");
			break;
		case Client::CCityObject::OP_INTERACT:
			pLayerTag = TEXT("layer_city_interact");
			break;
		case Client::CCityObject::OP_END:
			break;
		default:
			break;
		}

		wsprintf(szCityObjectTag, pCityObejctTag, i);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::CITY_OBJECT,
			pLayerTag, szCityObjectTag, &CityObject_Desc)))
		{
			MSG_BOX("Failed to Add_GameObject In Level_GamePlay : City_Object");
			return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

CLevel_City* CLevel_City::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_City* pInstance = new CLevel_City(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_City";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_City::Free()
{
	__super::Free();
}
