#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Test.h"
#include "Level_AnimTool.h"
#include "BackGround.h"
#include "Intro.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID eNextLevel)
{
	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//���� �̵��ϸ� �� ������ ��� ����Ʈ�� ������Ű��
	pGameInstance->Stop_All_Effect();

	if (pGM->GetCurrentLevel() >= LEVEL_LOGO)
	{
		if (FAILED(pGameInstance->Add_GameObjectEx(&m_pLoading, LEVEL_LOADING, OBJECT::BACKGROUND, L"layher_background", L"background")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pGameInstance->Add_GameObjectEx(&m_pLoading, LEVEL_LOADING, OBJECT::INTRO, L"layher_background", L"background")))
			return E_FAIL;
	}

	return S_OK;
}

void CLevel_Loading::Tick(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CAppManager* pApp = CAppManager::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

#ifdef _DEBUG
	wstring strTitleText = L"LOADING ";
	pApp->SetTitle(strTitleText.append(m_pLoader->GetLoadingStateText()));
#endif

	if (nullptr == m_pLoader)
		return;

	if (pGM->GetCurrentLevel() >= LEVEL_LOGO)
	{
		_float fLoadingRatio = pApp->GetCurrentLoadRatio();
		static_cast<CBackGround*>(m_pLoading)->SetLoadRatio(fLoadingRatio);
	}
	else
	{
		_float fLoadingRatio = pApp->GetCurrentLoadRatio();
		static_cast<CIntro*>(m_pLoading)->SetLoadRatio(fLoadingRatio);
	}

	if (true == m_pLoader->IsFinished())
	{
		CLevel*	pLevel = { nullptr };

		pGM->SetCurrentLevel(m_eNextLevel);

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_TEST:

			pLevel = CLevel_Test::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_ANIMTOOL:
			pLevel = CLevel_AnimTool::Create(m_pDevice, m_pContext);
			break;
		}
		if (nullptr == pLevel)
			return;

		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
			return;

		pApp->ResetLoadRatio();

		return;
	}

}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL_ID eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		wstring message = L"Failed to Create : CLevel_Loading";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();
	Safe_Release(m_pLoader);
}
