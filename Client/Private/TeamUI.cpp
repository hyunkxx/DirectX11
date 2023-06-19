#include "stdafx.h"
#include "..\Public\TeamUI.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "AppManager.h"

#include "Inventory.h"
#include "PlayerState.h"
#include "EchoSystem.h"
#include "CameraMovement.h"

#include "ItemDB.h"
#include "TerminalUI.h"

CTeamUI::CTeamUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTeamUI::CTeamUI(const CTeamUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTeamUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTeamUI::Initialize(void * pArg)
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

	m_OrthoTeamTitle.fWidth = 512.f * 0.65f;
	m_OrthoTeamTitle.fHeight = 64.f * 0.65f;
	m_OrthoTeamTitle.fX = 330.f;
	m_OrthoTeamTitle.fY = 80.f;
	CAppManager::ComputeOrtho(&m_OrthoTeamTitle);

	m_OrthoPartyBack.fWidth = 1100.f;
	m_OrthoPartyBack.fHeight = 420.f;
	m_OrthoPartyBack.fX = g_iWinSizeX >> 1;
	m_OrthoPartyBack.fY = (g_iWinSizeY >> 1) - 40.f;
	CAppManager::ComputeOrtho(&m_OrthoPartyBack);

	m_OrthoTeamSubTitle.fWidth = 512.f * 0.65f;
	m_OrthoTeamSubTitle.fHeight = 64.f * 0.65f;
	m_OrthoTeamSubTitle.fX = 330.f;
	m_OrthoTeamSubTitle.fY = 180.f;
	CAppManager::ComputeOrtho(&m_OrthoTeamSubTitle);

	m_OrthoCharImage[0].fWidth = 696.f * 0.47f;
	m_OrthoCharImage[0].fHeight = 960.f * 0.47f;
	m_OrthoCharImage[0].fX = (g_iWinSizeX >> 2);
	m_OrthoCharImage[0].fY = (g_iWinSizeY >> 1) + 60.f;
	CAppManager::ComputeOrtho(&m_OrthoCharImage[0]);

	m_OrthoCharImage[1].fWidth = 696.f * 0.47f;
	m_OrthoCharImage[1].fHeight = 960.f * 0.47f;
	m_OrthoCharImage[1].fX = (g_iWinSizeX >> 1);
	m_OrthoCharImage[1].fY = (g_iWinSizeY >> 1) + 60.f;
	CAppManager::ComputeOrtho(&m_OrthoCharImage[1]);

	m_OrthoCharImage[2].fWidth = 696.f * 0.47f;
	m_OrthoCharImage[2].fHeight = 960.f * 0.47f;
	m_OrthoCharImage[2].fX = g_iWinSizeX - (g_iWinSizeX * 0.25f);
	m_OrthoCharImage[2].fY = (g_iWinSizeY >> 1) + 60.f;
	CAppManager::ComputeOrtho(&m_OrthoCharImage[2]);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoCharName[i].fWidth = 512.f * 0.4f;
		m_OrthoCharName[i].fHeight = 64.f * 0.4f;
		m_OrthoCharName[i].fX = m_OrthoCharImage[i].fX - 40.f;
		m_OrthoCharName[i].fY = (g_iWinSizeY >> 1);
		CAppManager::ComputeOrtho(&m_OrthoCharName[i]);

		m_OrthoCharGage[i].fWidth = 100.f;
		m_OrthoCharGage[i].fHeight = 4.f;
		m_OrthoCharGage[i].fX = m_OrthoCharImage[i].fX - 92.f;
		m_OrthoCharGage[i].fY = (g_iWinSizeY >> 1) + 10.f;
		CAppManager::ComputeOrtho(&m_OrthoCharGage[i]);

		m_OrthoCharIcon[i].fWidth = 40.f;
		m_OrthoCharIcon[i].fHeight = 40.f;
		m_OrthoCharIcon[i].fX = m_OrthoCharImage[i].fX - 100.f;
		m_OrthoCharIcon[i].fY = g_iWinSizeY - 100.f;
		CAppManager::ComputeOrtho(&m_OrthoCharIcon[i]);

		m_OrthoCharLevelText[i].fWidth = 512.f * 0.6f;
		m_OrthoCharLevelText[i].fHeight = 64.f * 0.6f;
		m_OrthoCharLevelText[i].fX = m_OrthoCharImage[i].fX - 50.f;
		m_OrthoCharLevelText[i].fY = g_iWinSizeY - 100.f;
		CAppManager::ComputeOrtho(&m_OrthoCharLevelText[i]);

		for (_uint iDigit = 0; iDigit < 2; ++iDigit)
		{
			m_OrthoCharLevel[i][iDigit].fWidth = 16.f * 0.8f;
			m_OrthoCharLevel[i][iDigit].fHeight = 25.f * 0.8f;
			m_OrthoCharLevel[i][iDigit].fX = m_OrthoCharImage[i].fX - 5 + (iDigit * (15.f * 0.8f));
			m_OrthoCharLevel[i][iDigit].fY = g_iWinSizeY - 101.f;
			CAppManager::ComputeOrtho(&m_OrthoCharLevel[i][iDigit]);
		}
	
	}

#pragma endregion

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));

	return S_OK;
}

void CTeamUI::Start()
{
}

void CTeamUI::PreTick(_double TimeDelta)
{
	CGameObject::PreTick(TimeDelta);
}

void CTeamUI::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_bActive)
	{
		//keyInput(TimeDelta);
		elemAlphaUpdate(TimeDelta);
	}
}

void CTeamUI::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);

	if (m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTeamUI::Render()
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

HRESULT CTeamUI::RenderShadow()
{
	return S_OK;
}

void CTeamUI::RenderGUI()
{
}

void CTeamUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);
}

void CTeamUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();
}

void CTeamUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CTeamUI::addComponents()
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

void CTeamUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;

	elemAlphaReset(0);
	m_bElemAlphaStart[0] = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_Open_2.wem.wav", SOUND_UI_FEEDBACK, VOLUME_VFX);
}

void CTeamUI::stateDisable()
{
	m_bRender = false;
}

void CTeamUI::elemAlphaUpdate(_double TimeDelta)
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

void CTeamUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}
}

HRESULT CTeamUI::mainRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CPlayerState::PLAYER_STATE* pState = m_pPlayerState->Get_PlayerState();
	_uint iActiveCharCount = pState->iCharCount;
	
	_uint iCharLevel[3];
	iCharLevel[0] = m_pPlayerState->Get_CharState_byChar(0)->iCurLevel;
	iCharLevel[1] = m_pPlayerState->Get_CharState_byChar(1)->iCurLevel;
	iCharLevel[2] = m_pPlayerState->Get_CharState_byChar(2)->iCurLevel;

	_float fCharHP[3];
	fCharHP[0] = m_pPlayerState->Get_CharState_byChar(0)->fCurHP;
	fCharHP[1] = m_pPlayerState->Get_CharState_byChar(1)->fCurHP;
	fCharHP[2] = m_pPlayerState->Get_CharState_byChar(2)->fCurHP;

	_float fCharMaxHP[3];
	fCharMaxHP[0] = m_pPlayerState->Get_CharState_byChar(0)->fMaxHP;
	fCharMaxHP[1] = m_pPlayerState->Get_CharState_byChar(1)->fMaxHP;
	fCharMaxHP[2] = m_pPlayerState->Get_CharState_byChar(2)->fMaxHP;

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };
	_float fMainAlpha = 1.f;

	// 메인 백그라운드
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fMainAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_DRAGON_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 메인 타이틀
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTeamTitle.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TEAM_TITLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 메인 패널
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPartyBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_PANEL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 서브 타이틀
	fAlpha = m_fElemAlpha[1] * 0.3f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTeamSubTitle.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	vColor3 = SLOT_LEGEND_COLOR;
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTeamSubTitle.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TEAM_SUB_TITLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < 3; ++i)
	{
		//캐릭터 이미지
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharImage[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_CHAR_ROVER + i, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		if (iActiveCharCount > i)
			m_pShader->Begin(10);
		else
			m_pShader->Begin(12);

		m_pVIBuffer->Render();

		if (iActiveCharCount > i)
		{
			// 캐릭터이름
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharName[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_ROVER + i, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			// 캐릭터 게이지
			vColor3 = { 0.1f, 0.1f, 0.1f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharGage[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(8);
			m_pVIBuffer->Render();

			_float fValue = fCharHP[i] / fCharMaxHP[i];
			vColor3 = { 0.2f, 0.6f, 0.2f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &fValue, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharGage[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(15);
			m_pVIBuffer->Render();

			// 캐릭터 속성
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharIcon[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_ROVER_ATTRIBUTE + i, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			// 캐릭터 레벨 텍스트
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharLevelText[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEVEL_TEXT, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			// 캐릭터 레벨
			string strLevel = to_string(iCharLevel[i]);
			_uint iDigit = (_uint)strLevel.size();

			for (_uint iIndex = 0; iIndex < iDigit; ++iIndex)
			{
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharLevel[i][iIndex].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(strLevel[iIndex] - '0', m_pShader, "g_DiffuseTexture")))
					return E_FAIL;

				m_pShader->Begin(10);
				m_pVIBuffer->Render();
			}
		}

	}

	return S_OK;
}

CTeamUI * CTeamUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTeamUI* pInstance = new CTeamUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTeamUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTeamUI::Clone(void * pArg)
{
	CTeamUI* pInstance = new CTeamUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTeamUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTeamUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
