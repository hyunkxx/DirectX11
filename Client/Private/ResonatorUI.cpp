#include "stdafx.h"
#include "..\Public\ResonatorUI.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "TerminalUI.h"
#include "PlayerState.h"

CResonatorUI::CResonatorUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CResonatorUI::CResonatorUI(const CResonatorUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CResonatorUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CResonatorUI::Initialize(void * pArg)
{
	CAppManager* pApp = CAppManager::GetInstance();

	assert(pArg);
	m_pTerminal = static_cast<CTerminalUI*>(pArg);

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	ZeroMemory(m_OrthoButtons, sizeof(ORTHO_DESC) * BTN_END);
	ZeroMemory(m_fButtonAlpha, sizeof(_float) * BTN_END);

	// Ortho Settings
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

#pragma region FRAME_ORTHO
	// Background
	m_OrthoBackground.fWidth = g_iWinSizeX;
	m_OrthoBackground.fHeight = g_iWinSizeY;
	m_OrthoBackground.fX = g_iWinSizeX >> 1;
	m_OrthoBackground.fY = g_iWinSizeY >> 1;
	pApp->ComputeOrtho(&m_OrthoBackground);

	// SlotText
	m_OrthoTitleText.fWidth = 512.f * 0.7f;
	m_OrthoTitleText.fHeight = 64.f * 0.7f;
	m_OrthoTitleText.fX = 320.f;
	m_OrthoTitleText.fY = 90.f;
	pApp->ComputeOrtho(&m_OrthoTitleText);

#pragma endregion

#pragma region STATE_DETAILS
	// PlayerText
	m_OrthoPlayerText.fWidth = 512.f * 0.6f;
	m_OrthoPlayerText.fHeight = 64.f * 0.6f;
	m_OrthoPlayerText.fX = 320.f;
	m_OrthoPlayerText.fY = 185.f;
	pApp->ComputeOrtho(&m_OrthoPlayerText);

	// LevelText
	m_OrthoLevelText.fWidth = 512.f * 0.6f;
	m_OrthoLevelText.fHeight = 64.f * 0.6f;
	m_OrthoLevelText.fX = 320.f;
	m_OrthoLevelText.fY = 175.f + 64.f * 0.6f;
	pApp->ComputeOrtho(&m_OrthoLevelText);

	// CurLevel
	for (_uint i = 0; i < 5; ++i)
	{
		m_OrthoLevel[i].fWidth = 16.f * 0.8f;
		m_OrthoLevel[i].fHeight = 25.f * 0.8f;
		m_OrthoLevel[i].fX = 230.f + (i * (18.f * 0.7f));
		m_OrthoLevel[i].fY = 173.f + 64.f * 0.6f;
		pApp->ComputeOrtho(&m_OrthoLevel[i]);
	}

	// Exp
	m_OrthoExp.fWidth = 220.f;
	m_OrthoExp.fHeight = 8.f;
	m_OrthoExp.fX = 270.f;
	m_OrthoExp.fY = 200.f + 64.f * 0.6f;
	pApp->ComputeOrtho(&m_OrthoExp);

	// Buttons
	for (_uint i = 0; i < BTN_END; ++i)
	{
		//Slot
		m_OrthoButtons[i].fWidth = 90.f;
		m_OrthoButtons[i].fHeight = 90.f;
		m_OrthoButtons[i].fX = 90.f;
		m_OrthoButtons[i].fY = 200.f + (i * 100.f);
		pApp->ComputeOrtho(&m_OrthoButtons[i]);

		// Icon
		m_OrthoButtonIcons[i].fWidth = 65.f;
		m_OrthoButtonIcons[i].fHeight = 65.f;
		m_OrthoButtonIcons[i].fX = 90.f;
		m_OrthoButtonIcons[i].fY = 202.f + (i * 100.f);
		pApp->ComputeOrtho(&m_OrthoButtonIcons[i]);
	}

	// HP, Attack, Defense, Crit
	const _float fRatio = 0.4f;
	const _float fOffset = 5.f;

	m_OrthoState[0].fWidth = 400.f * fRatio;
	m_OrthoState[0].fHeight = 150.f * fRatio;
	m_OrthoState[0].fX = 240.f;
	m_OrthoState[0].fY = 300.f;
	pApp->ComputeOrtho(&m_OrthoState[0]);

	m_OrthoState[1].fWidth = 400.f * fRatio;
	m_OrthoState[1].fHeight = 150.f * fRatio;
	m_OrthoState[1].fX = m_OrthoState[0].fX + (m_OrthoState[0].fWidth) + fOffset;
	m_OrthoState[1].fY = 300.f;
	pApp->ComputeOrtho(&m_OrthoState[1]);

	m_OrthoState[2].fWidth = 400.f * fRatio;
	m_OrthoState[2].fHeight = 150.f * fRatio;
	m_OrthoState[2].fX = m_OrthoState[0].fX;
	m_OrthoState[2].fY = 300.f + (m_OrthoState[0].fHeight + fOffset);
	pApp->ComputeOrtho(&m_OrthoState[2]);

	m_OrthoState[3].fWidth = 400.f * fRatio;
	m_OrthoState[3].fHeight = 150.f * fRatio;
	m_OrthoState[3].fX = m_OrthoState[0].fX + (m_OrthoState[0].fWidth) + fOffset;
	m_OrthoState[3].fY = 300.f + (m_OrthoState[0].fHeight + fOffset);
	pApp->ComputeOrtho(&m_OrthoState[3]);

	// StateData 출력 최대 6자리
	for (_uint i = 0; i < STATE_END; ++i)
	{
		for (_uint digit = 0; digit < 6; ++digit)
		{
			m_OrthoStateData[i][digit].fWidth = 12.f;
			m_OrthoStateData[i][digit].fHeight = 19.f;
			m_OrthoStateData[i][digit].fX = m_OrthoState[i].fX - 30.f + (digit * 12.f);
			m_OrthoStateData[i][digit].fY = m_OrthoState[i].fY + 7.f;

			pApp->ComputeOrtho(&m_OrthoStateData[i][digit]);
		}
	}

#pragma endregion

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));

	return S_OK;
}

void CResonatorUI::Start()
{
}

void CResonatorUI::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	if (pGameInstance->InputKey(DIK_B) == KEY_STATE::TAP)
	{
		m_pPlayerState->AddExp(CPlayerState::CHARACTER_ROVER, 10.f);
	}

	stateUpdate(TimeDelta);
}

void CResonatorUI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if(m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CResonatorUI::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	if (FAILED(defaultSlotRender()))
		return E_FAIL;

	switch (m_eCurButton)
	{
	case CResonatorUI::BTN_STATE:
		if (FAILED(stateRender()))
			return E_FAIL;

		if (FAILED(stateData()))
			return E_FAIL;

		break;
	case CResonatorUI::BTN_WEAPON:
		break;
	case CResonatorUI::BTN_ECHO:
		break;
	case CResonatorUI::BTN_RESONANCE:
		break;
	case CResonatorUI::BTN_WUTERIDE:
		break;
	case CResonatorUI::BTN_END:
		break;
	}

	if (FAILED(m_pShader->SetRawValue("g_vColor", &_float3(1.f, 1.f, 1.f), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

void CResonatorUI::RenderGUI()
{
}

void CResonatorUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);
}

void CResonatorUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();

}

void CResonatorUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CResonatorUI::addComponents()
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

void CResonatorUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;
	m_fBackgroundAlpha = 1.f;
	m_bButtonActive[0] = true;
	m_eCurButton = BTN_STATE;
}

void CResonatorUI::stateDisable()
{

}

void CResonatorUI::stateUpdate(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bActive)
	{
		for (_uint i = 0; i < BTN_END; ++i)
		{
			if (m_bButtonActive[i])
			{
				m_fButtonAlpha[i] += (_float)TimeDelta * 2.f;

				if (pGameMode->OnMouse(m_OrthoButtons[i]) 
					&& pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					m_eCurButton = (RESONATOR_BTN)i;
				}

			}

			if (m_fButtonAlpha[i] >= 0.5f && i < BTN_END - 1)
				m_bButtonActive[i + 1] = true;

			if (m_fButtonAlpha[i] > 1.f)
				m_fButtonAlpha[i] = 1.f;
		}
	}
	else
	{
		for (_uint i = 0; i < BTN_END; ++i)
		{
			m_fBackgroundAlpha = m_fButtonAlpha[i] -= (_float)TimeDelta;
			if (m_fButtonAlpha[i] < 1.f)
			{
				m_fBackgroundAlpha = 0.f;
				m_fButtonAlpha[i] = 0.f;
				m_bButtonActive[i] = false;
				m_bRender = false;
			}

		}
	}
}

_uint CResonatorUI::getCurSlotText(RESONATOR_BTN eButton)
{
	_uint iTextureID = 0;

	switch (eButton)
	{
	case BTN_STATE:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_STATETEXT;
		break;
	case BTN_WEAPON:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_WEAPONTEXT;
		break;
	case BTN_ECHO:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_STATETEXT;
		break;
	case BTN_RESONANCE:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_STATETEXT;
		break;
	case BTN_WUTERIDE:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_STATETEXT;
		break;
	}

	return iTextureID;
}

_uint CResonatorUI::getCurSlotIcon(RESONATOR_BTN eButton)
{
	_uint iTextureID = 0;

	switch (eButton)
	{
	case BTN_STATE:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_STATE;
		break;
	case BTN_WEAPON:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_WEAPON;
		break;
	case BTN_ECHO:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_ECHO;
		break;
	case BTN_RESONANCE:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_RENONANCE;
		break;
	case BTN_WUTERIDE:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_WUTHERIDE;
		break;
	}

	return iTextureID;
}

_uint CResonatorUI::getStateImage(_uint iIndex)
{
	_uint iTextureID = 0;

	switch (iIndex)
	{
	case 0:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_HP;
		break;
	case 1:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_ATTACK;
		break;
	case 2:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_DEFENSE;
		break;
	case 3:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_CRITRATE;
		break;
	}

	return iTextureID;
}

HRESULT CResonatorUI::defaultSlotRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Background
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fBackgroundAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_DETAILPANEL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// ButtonTitleText
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitleText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(getCurSlotText(m_eCurButton), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// BTN
	for (_uint i = 0; i < BTN_END; ++i)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoButtons[i].WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[i], sizeof(_float))))
			return E_FAIL;

		if (m_eCurButton == i)
		{
			_float3 vColor = UNIQUE_COLOR;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
		}
		else
		{
			_float3 vColor = DEFAULT_COLOR;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
		}


		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoButtonIcons[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(getCurSlotIcon((RESONATOR_BTN)i), m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

HRESULT CResonatorUI::stateRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// State
	for (_uint i = 0; i < STATE_END; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[i], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoState[i].WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_LISTBACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(pGameInstance->SetupSRV(getStateImage(i), m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// Player Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPlayerText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_PLAYER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Level Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevelText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_LEVEL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	CPlayerState::CHARACTER_STATE* pPlayerState = m_pPlayerState->Get_CharState_bySlot(CPlayerState::SLOT_MAIN);
	string strCurLevel = to_string((_int)pPlayerState->iCurLevel);
	_uint iLevelDigit = (_uint)strCurLevel.size();

	//CurLevel
	for (_uint i = 0; i < iLevelDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCurLevel[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// 슬래쉬
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[iLevelDigit].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SLASH, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	// MaxLevel = 50
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[iLevelDigit + 1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_5, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[iLevelDigit + 2].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_0, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Exp Gage Back
	_float3 vColor = { 0.7f, 0.7f, 0.7f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoExp.WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(9);
	m_pVIBuffer->Render();

	_float fFill = pPlayerState->fCurExp / pPlayerState->fMaxExp;

	// ExpGage Front
	if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &fFill, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoExp.WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(2);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CResonatorUI::stateData()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CPlayerState::CHARACTER_STATE* pPlayerState = m_pPlayerState->Get_CharState_bySlot(CPlayerState::SLOT_MAIN);

	// 연산된 최종 스텟 추후에 현재 선택된 캐릭터 파라미터로 던져서 스텟 보여주기
	string strData[4];
	strData[0] = to_string((_int)pPlayerState->fMaxHP);
	strData[1] = to_string((_int)pPlayerState->fAttack[0]);
	strData[2] = to_string((_int)pPlayerState->fDefense[0]);
	strData[3] = to_string((_int)pPlayerState->fCriticalRate[0]);

	// State
	for (_uint i = 0; i < STATE_END; ++i)
	{
		// State Data
		_uint dataSize = (_uint)strData[i].size();
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[i], sizeof(_float))))
			return E_FAIL;

		for (_uint iDigit = 0; iDigit < dataSize; ++iDigit)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoStateData[i][iDigit].WorldMatrix)))
				return E_FAIL;

			_uint dataNumber = strData[i][iDigit] - '0';
			if (FAILED(pGameInstance->SetupSRV(dataNumber, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
	}

	// 퍼센트 붙히기
	_uint iPerIndex = (_uint)strData[3].size();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoStateData[STATE_CRITRATE][iPerIndex].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	return S_OK;
}

CResonatorUI * CResonatorUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CResonatorUI*	pInstance = new CResonatorUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CResonatorUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CResonatorUI::Clone(void * pArg)
{
	CResonatorUI*	pInstance = new CResonatorUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CResonatorUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CResonatorUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

}