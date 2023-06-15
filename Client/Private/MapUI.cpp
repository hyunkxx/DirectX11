#include "stdafx.h"
#include "..\Public\MapUI.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "AppManager.h"

#include "Inventory.h"
#include "PlayerState.h"
#include "EchoSystem.h"
#include "CameraMovement.h"

#include "ItemDB.h"
#include "TerminalUI.h"

CMapUI::CMapUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapUI::CMapUI(const CMapUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapUI::Initialize(void * pArg)
{
	CAppManager* pApp = CAppManager::GetInstance();

	assert(pArg);
	m_pTerminal = static_cast<CTerminalUI*>(pArg);

	ZeroMemory(m_fElemAlpha, sizeof(m_fElemAlpha));
	ZeroMemory(m_bElemAlphaStart, sizeof(m_bElemAlphaStart));

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	// Ortho Settings
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

#pragma region ORTHO_SETTUP
	m_OrthoBackground.fWidth = g_iWinSizeX;
	m_OrthoBackground.fHeight = g_iWinSizeY;
	m_OrthoBackground.fX = g_iWinSizeX >> 1;
	m_OrthoBackground.fY = g_iWinSizeY >> 1;
	CAppManager::ComputeOrtho(&m_OrthoBackground);

	m_OrthoTitle.fWidth = 512.f * 0.8f;
	m_OrthoTitle.fHeight = 64.f * 0.8f;
	m_OrthoTitle.fX = g_iWinSizeX >> 1;
	m_OrthoTitle.fY = 40.f;
	CAppManager::ComputeOrtho(&m_OrthoTitle);

	m_OrthoMap.fWidth  = 1024.f * 0.6f;
	m_OrthoMap.fHeight = 1024.f * 0.6f;
	m_OrthoMap.fX = (g_iWinSizeX >> 2) + 100.f;
	m_OrthoMap.fY = g_iWinSizeY >> 1;
	CAppManager::ComputeOrtho(&m_OrthoMap);

	m_OrthoForestIcon.fWidth  = 60.f;
	m_OrthoForestIcon.fHeight = 60.f;
	m_OrthoForestIcon.fX = (g_iWinSizeX >> 2) + 75.f;
	m_OrthoForestIcon.fY = (g_iWinSizeY >> 1) + 130.f;
	CAppManager::ComputeOrtho(&m_OrthoForestIcon);

	m_OrthoCityIcon.fWidth  = 60.f;
	m_OrthoCityIcon.fHeight = 60.f;
	m_OrthoCityIcon.fX = (g_iWinSizeX >> 2) + 125.f;
	m_OrthoCityIcon.fY = (g_iWinSizeY >> 1) - 115.f;
	CAppManager::ComputeOrtho(&m_OrthoCityIcon);

	m_OrthoValleyIcon.fWidth = 60.f;
	m_OrthoValleyIcon.fHeight = 60.f;
	m_OrthoValleyIcon.fX = (g_iWinSizeX >> 1) - 50.f;
	m_OrthoValleyIcon.fY = 180.f;
	CAppManager::ComputeOrtho(&m_OrthoValleyIcon);

	m_OrthoCrownIcon.fWidth = 60.f;
	m_OrthoCrownIcon.fHeight = 60.f;
	m_OrthoCrownIcon.fX = 180.f;
	m_OrthoCrownIcon.fY = g_iWinSizeY - 320.f;
	CAppManager::ComputeOrtho(&m_OrthoCrownIcon);

	m_OrthoPreView.fWidth = 1536.f * 0.23f;
	m_OrthoPreView.fHeight = 864.f * 0.23f;
	m_OrthoPreView.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2) - 10.f;
	m_OrthoPreView.fY = (g_iWinSizeY>>1) - 80.f;
	CAppManager::ComputeOrtho(&m_OrthoPreView);

	m_OrthoPreDescBack.fWidth  = 720.f * 0.7f;
	m_OrthoPreDescBack.fHeight = 360.f * 0.55f;
	m_OrthoPreDescBack.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2) - 20.f;
	m_OrthoPreDescBack.fY = g_iWinSizeY - 245.f;
	CAppManager::ComputeOrtho(&m_OrthoPreDescBack);

	m_OrthoPreDesc.fWidth = 720.f * 0.4f;
	m_OrthoPreDesc.fHeight = 360.f * 0.4f;
	m_OrthoPreDesc.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2);
	m_OrthoPreDesc.fY = g_iWinSizeY - 245.f;
	CAppManager::ComputeOrtho(&m_OrthoPreDesc);

	m_OrthoMapName.fWidth = 512.f * 0.65f;
	m_OrthoMapName.fHeight = 64.f * 0.65f;
	m_OrthoMapName.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2);
	m_OrthoMapName.fY = (g_iWinSizeY >> 1) - 175.f;
	CAppManager::ComputeOrtho(&m_OrthoMapName);

	m_OrthoButton.fWidth = 200.f;
	m_OrthoButton.fHeight = 70.f;
	m_OrthoButton.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2);
	m_OrthoButton.fY = g_iWinSizeY  - 130.f;
	CAppManager::ComputeOrtho(&m_OrthoButton);

	m_OrthoButtonText.fWidth = 512.f * 0.6f;
	m_OrthoButtonText.fHeight = 64.f * 0.6f;
	m_OrthoButtonText.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2);
	m_OrthoButtonText.fY = g_iWinSizeY - 128.f;
	CAppManager::ComputeOrtho(&m_OrthoButtonText);

	m_OrthoCoinBack.fWidth = 350.f;
	m_OrthoCoinBack.fHeight = 30.f;
	m_OrthoCoinBack.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2);
	m_OrthoCoinBack.fY = 50.f;
	CAppManager::ComputeOrtho(&m_OrthoCoinBack);

	m_OrthoCoinIcon.fWidth = 30.f;
	m_OrthoCoinIcon.fHeight = 30.f;
	m_OrthoCoinIcon.fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2) - 90.f;
	m_OrthoCoinIcon.fY = 50.f;
	CAppManager::ComputeOrtho(&m_OrthoCoinIcon);

	for (_uint i = 0; i < 8; ++i)
	{
		m_OrthoCoinAmount[i].fWidth = 16.f  * 0.8f;
		m_OrthoCoinAmount[i].fHeight = 25.f * 0.8f;
		m_OrthoCoinAmount[i].fX = (g_iWinSizeX >> 1) + (g_iWinSizeX >> 2) - 70.f + (i * (15.f * 0.8f));
		m_OrthoCoinAmount[i].fY = 50.f;
		CAppManager::ComputeOrtho(&m_OrthoCoinAmount[i]);
	}


#pragma endregion

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));

	return S_OK;
}

void CMapUI::Start()
{
}

void CMapUI::PreTick(_double TimeDelta)
{
	CGameObject::PreTick(TimeDelta);
}

void CMapUI::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_bActive)
	{
		keyInput(TimeDelta);
		elemAlphaUpdate(TimeDelta);
	}
}

void CMapUI::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);

	if (m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMapUI::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	mainRender();

	if (FAILED(m_pShader->SetRawValue("g_vColor", &_float3(1.f, 1.f, 1.f), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapUI::RenderShadow()
{
	return S_OK;
}

void CMapUI::RenderGUI()
{
}

void CMapUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);
}

void CMapUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();
}

void CMapUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CMapUI::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI_SUB,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CMapUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;

	elemAlphaReset(0);
	m_bElemAlphaStart[0] = true;
	m_eLevel = SELECT_END;
}

void CMapUI::stateDisable()
{
	m_bRender = false;
}

void CMapUI::elemAlphaUpdate(_double TimeDelta)
{
	for (_uint i = 0; i < SMOOTH_MAX; ++i)
	{
		if (m_bElemAlphaStart[i])
		{
			m_fElemAlpha[i] += (_float)TimeDelta * 2.f;

			if (m_fElemAlpha[i] >= 1.f)
			{
				m_fElemAlpha[i] = 1.f;
			}
		}

		if (m_fElemAlpha[i] >= 0.3f)
		{
			if (i < SMOOTH_MAX - 1)
				m_bElemAlphaStart[i + 1] = true;

		}
	}
}

void CMapUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}
}

void CMapUI::keyInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (pGM->OnMouse(m_OrthoValleyIcon))
	{
		if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			if (m_eLevel != VALLEY)
				m_fElemAlpha[6] = 0.f;

			m_eLevel = VALLEY;
		}
	}
	else if (pGM->OnMouse(m_OrthoCityIcon))
	{
		if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			if (m_eLevel != CITY)
				m_fElemAlpha[6] = 0.f;

			m_eLevel = CITY;
		}
	}
	else if (pGM->OnMouse(m_OrthoForestIcon))
	{
		if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			if (m_eLevel != FOREST)
				m_fElemAlpha[6] = 0.f;

			m_eLevel = FOREST;
		}
	}
	else if (pGM->OnMouse(m_OrthoCrownIcon))
	{
		if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			if (m_eLevel != CROWN)
				m_fElemAlpha[6] = 0.f;

			m_eLevel = CROWN;
		}
	}
	else  if (pGM->OnMouse(m_OrthoButton))
	{
		if (pGI->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			m_bButtonPush = true;
		else
			m_bButtonPush = false;

		if (pGI->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
		{
			if (m_eLevel != SELECT_END)
			{
				pGM->ReserveLevel((LEVEL_ID)m_eLevel);
			}
		}
	}
	else
	{
		m_bButtonPush = false;
	}
}

HRESULT CMapUI::mainRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };
	_float fMainAlpha = 1.f;

	// 메인 백그라운드
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fMainAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_TELEPORT_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	fAlpha = 0.8f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GRADBACKGROUND, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 타이틀
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitle.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::GLOW_PUPLE_LINE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitle.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_TELEPORT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	if (m_eLevel != SELECT_END)
	{
		vColor3 = { 0.3f, 0.3f, 0.3f };
		fAlpha = m_fElemAlpha[4] * 0.5f;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreDescBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(13);
		m_pVIBuffer->Render();
	}

	// 맵 이미지
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMap.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_MAP, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//용계 아이콘
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoValleyIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_ICON_BATTLE_NONE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//도시 아이콘
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCityIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_ICON_CITY_NONE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//숲 아이콘
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoForestIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_ICON_BATTLE_NONE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//크라운 아이콘
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCrownIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_ICON_CROWN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	//선택된 레벨
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	switch (m_eLevel)
	{
	case Client::CMapUI::VALLEY:
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoValleyIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreView.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_IMAGE_DRAGON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// 맵 이름
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMapName.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_VALLEY, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_DESC_DRAGON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(14);
		m_pVIBuffer->Render();

		break;
	case Client::CMapUI::CITY:
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCityIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreView.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_IMAGE_CITY, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// 맵 이름
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMapName.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_VILLAGE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_DESC_CITY, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(14);
		m_pVIBuffer->Render();
		break;
	case Client::CMapUI::FOREST:
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoForestIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreView.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_IMAGE_FOREST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// 맵 이름
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMapName.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_FOREST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_DESC_FOREST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(14);
		m_pVIBuffer->Render();
		break;
	case Client::CMapUI::CROWN:
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCrownIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreView.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_IMAGE_CROWN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// 맵 이름
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMapName.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_CROWNS_NEST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPreDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::MAP_DESC_CROWN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(14);
		m_pVIBuffer->Render();
		break;
	}

	if (m_eLevel != SELECT_END)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoButton.WorldMatrix)))
			return E_FAIL;

		if (!m_bButtonPush)
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_NONE, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_CLICK, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoButtonText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_CONFIRM, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	vColor3 = { 0.3f, 0.3f, 0.3f };
	fAlpha = m_fElemAlpha[3] * 0.5f;
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCoinBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(13);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCoinIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_TACTITE_COIN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	string strNumber = to_string(m_pInven->GetCoin());
	_uint iDigit = (_uint)strNumber.size();
	vColor3 = { 0.8f, 0.8f, 0.8f };
	for (_uint i = 0; i < iDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCoinAmount[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strNumber[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}


	return S_OK;
}

CMapUI * CMapUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapUI* pInstance = new CMapUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapUI::Clone(void * pArg)
{
	CMapUI* pInstance = new CMapUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
