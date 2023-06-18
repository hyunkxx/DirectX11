#include "stdafx.h"
#include "..\Public\SettingUI.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "AppManager.h"

#include "Inventory.h"
#include "PlayerState.h"
#include "EchoSystem.h"
#include "CameraMovement.h"

#include "ItemDB.h"
#include "TerminalUI.h"

CSettingUI::CSettingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSettingUI::CSettingUI(const CSettingUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSettingUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSettingUI::Initialize(void * pArg)
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
	
	m_OrthoTitle.fWidth = 512.f * 0.6f;
	m_OrthoTitle.fHeight = 64.f * 0.6f;
	m_OrthoTitle.fX = 200.f;
	m_OrthoTitle.fY = 80.f;
	CAppManager::ComputeOrtho(&m_OrthoTitle);

	//Graphics
	for (_uint i = 0; i < GRAPHIC_END; ++i)
	{
		m_OrthoGraphicSlot[i].fWidth = 512.f * 1.f;
		m_OrthoGraphicSlot[i].fHeight = 60.f;
		m_OrthoGraphicSlot[i].fX = 0.f;
		m_OrthoGraphicSlot[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicSlot[i]);

		m_OrthoGraphicText[i].fWidth = 512.f * 0.6f;
		m_OrthoGraphicText[i].fHeight = 64.f * 0.6f;
		m_OrthoGraphicText[i].fX = 0.f;
		m_OrthoGraphicText[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicText[i]);

		m_OrthoGraphicBtnLeft[i].fWidth = 40.f;
		m_OrthoGraphicBtnLeft[i].fHeight = 40.f;
		m_OrthoGraphicBtnLeft[i].fX = 0.f;
		m_OrthoGraphicBtnLeft[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeft[i]);

		m_OrthoGraphicBtnRight[i].fWidth = 40.f;
		m_OrthoGraphicBtnRight[i].fHeight = 40.f;
		m_OrthoGraphicBtnRight[i].fX = 0.f;
		m_OrthoGraphicBtnRight[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRight[i]);

		m_OrthoGraphicBtnLeftBtn[i].fWidth = 30.f;
		m_OrthoGraphicBtnLeftBtn[i].fHeight = 30.f;
		m_OrthoGraphicBtnLeftBtn[i].fX = 0.f;
		m_OrthoGraphicBtnLeftBtn[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeftBtn[i]);

		m_OrthoGraphicBtnRightBtn[i].fWidth = 30.f;
		m_OrthoGraphicBtnRightBtn[i].fHeight = 30.f;
		m_OrthoGraphicBtnRightBtn[i].fX = 0.f;
		m_OrthoGraphicBtnRightBtn[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRightBtn[i]);

		m_OrthoGraphicBtnText[i].fWidth = 512.f * 0.6f;
		m_OrthoGraphicBtnText[i].fHeight = 64.f * 0.6f;
		m_OrthoGraphicBtnText[i].fX = 0.f;
		m_OrthoGraphicBtnText[i].fY = 180.f + (i * 60.f);
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnText[i]);
	}

#pragma endregion

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));
	
	ZeroMemory(m_iGraphicState, sizeof(m_iGraphicState));
	m_iGraphicState[GRAPHIC_SHADOW] = 1;
	m_iGraphicState[GRAPHIC_SSAO] = 1;
	m_iGraphicState[GRAPHIC_OUTLINE] = 1;
	m_iGraphicState[GRAPHIC_FILTER] = CRenderer::LUT_FUJI;
	m_iGraphicState[GRAPHIC_FOG] = 1;

	return S_OK;
}

void CSettingUI::Start()
{
}

void CSettingUI::PreTick(_double TimeDelta)
{
	CGameObject::PreTick(TimeDelta);
}

void CSettingUI::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_bActive)
	{
		keyInput(TimeDelta);
		elemAlphaUpdate(TimeDelta);
	}
}

void CSettingUI::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);

	if (m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSettingUI::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	mainRender();

	graphicRender();

	if (FAILED(m_pShader->SetRawValue("g_vColor", &_float3(1.f, 1.f, 1.f), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSettingUI::RenderShadow()
{
	return S_OK;
}

void CSettingUI::RenderGUI()
{
}

void CSettingUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);
}

void CSettingUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();
}

void CSettingUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CSettingUI::addComponents()
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

void CSettingUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;

	elemAlphaReset(0);
	m_bElemAlphaStart[0] = true;
	ZeroMemory(m_fSmoothMove, sizeof m_fSmoothMove);

	for (_uint i = 0; i < GRAPHIC_END; ++i)
	{
		m_OrthoGraphicSlot[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicSlot[i]);

		m_OrthoGraphicText[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicText[i]);

		m_OrthoGraphicBtnLeft[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeft[i]);

		m_OrthoGraphicBtnRight[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRight[i]);
	}
}

void CSettingUI::stateDisable()
{
	m_bActive = false;
	m_bRender = false;
}

void CSettingUI::elemAlphaUpdate(_double TimeDelta)
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

	for (_uint i = 0; i < GRAPHIC_END; ++i)
	{
		// 0.f -> 240.f;
		if (m_fElemAlpha[i] >= 1.f)
		{
			_vector vSour = XMVectorSet(m_fSmoothMove[i], m_fSmoothMove[i], m_fSmoothMove[i], 1.f);
			_vector vDesc = XMVectorSet(240.f, 240.f, 240.f, 1.f);
			m_fSmoothMove[i] = XMVectorGetX(XMVectorLerp(vSour, vDesc, (_float)TimeDelta * 2.f));

			m_OrthoGraphicSlot[i].fX = m_fSmoothMove[i];
			CAppManager::ComputeOrtho(&m_OrthoGraphicSlot[i]);

			m_OrthoGraphicText[i].fX = m_fSmoothMove[i] + 20.f;
			CAppManager::ComputeOrtho(&m_OrthoGraphicText[i]);

			//버튼 슬롯
			m_OrthoGraphicBtnLeftBtn[i].fX = m_fSmoothMove[i] - 10.f;
			CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeftBtn[i]);
			m_OrthoGraphicBtnRightBtn[i].fX = m_fSmoothMove[i] + 150.f;
			CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRightBtn[i]);

			//버튼
			m_OrthoGraphicBtnLeft[i].fX = m_fSmoothMove[i] - 10.f;
			CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeft[i]);
			m_OrthoGraphicBtnRight[i].fX = m_fSmoothMove[i] + 150.f;
			CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRight[i]);

			m_OrthoGraphicBtnText[i].fX = m_fSmoothMove[i] + 70.f;
			CAppManager::ComputeOrtho(&m_OrthoGraphicBtnText[i]);
		}
	}

}

void CSettingUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}

	ZeroMemory(m_fSmoothMove, sizeof m_fSmoothMove);

	for (_uint i = 0; i < GRAPHIC_END; ++i)
	{
		m_OrthoGraphicSlot[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicSlot[i]);

		m_OrthoGraphicText[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicText[i]);

		m_OrthoGraphicBtnLeft[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeft[i]);

		m_OrthoGraphicBtnRight[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRight[i]);

		m_OrthoGraphicBtnLeftBtn[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnLeftBtn[i]);

		m_OrthoGraphicBtnRightBtn[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnRightBtn[i]);

		m_OrthoGraphicBtnText[i].fX = 0.f;
		CAppManager::ComputeOrtho(&m_OrthoGraphicBtnText[i]);

	}
}

void CSettingUI::keyInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (m_eCurType == SETTING_TYPE::SET_GRAPHIC)
		graphicInput(TimeDelta);
	else
		audioInput(TimeDelta);
}

void CSettingUI::graphicInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
	{
		for (_uint i = 0; i < GRAPHIC_END; ++i)
		{
			if (pGM->OnMouse(m_OrthoGraphicBtnLeftBtn[i]))
			{
				onColickGraphicLeftButton((GRAPHIC_BTN)i);
			}
			else if (pGM->OnMouse(m_OrthoGraphicBtnRightBtn[i]))
			{
				onColickGraphicRightButton((GRAPHIC_BTN)i);
			}

		}
		
	}
	else if (pGI->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
	{

	}
	else if (pGI->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
	{

	}

}

void CSettingUI::audioInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

}

void CSettingUI::onColickGraphicLeftButton(GRAPHIC_BTN eGraphicButton)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (eGraphicButton == GRAPHIC_FILTER)
	{
		if (m_iGraphicState[eGraphicButton] > 0)
			m_iGraphicState[eGraphicButton]--;
		else
			m_iGraphicState[eGraphicButton] = CRenderer::LUT_DEFAULT;
	}
	else
	{
		if (m_iGraphicState[eGraphicButton] > 0)
			m_iGraphicState[eGraphicButton]--;
	}

	switch (eGraphicButton)
	{
	case GRAPHIC_SHADOW:
		pGI->SetShadowLevel(SHADOW_LEVEL::SHADOW_OFF);
		break;
	case GRAPHIC_SSAO:
		pGI->SetSSAO(false);
		break;
	case GRAPHIC_OUTLINE:
		pGI->SetOutline(false);
		break;
	case GRAPHIC_FILTER:
		pGI->SetLUT((CRenderer::LUT)m_iGraphicState[GRAPHIC_FILTER]);
		break;
	case GRAPHIC_FOG:
		pGI->SetFog(false);
		break;
	}
}

void CSettingUI::onColickGraphicRightButton(GRAPHIC_BTN eGraphicButton)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (eGraphicButton == GRAPHIC_FILTER)
	{
		if (m_iGraphicState[eGraphicButton] < CRenderer::LUT_DEFAULT)
			m_iGraphicState[eGraphicButton]++;
		else
			m_iGraphicState[eGraphicButton] = 0;
	}
	else
	{
		if (m_iGraphicState[eGraphicButton] < 1)
			m_iGraphicState[eGraphicButton]++;
	}

	switch (eGraphicButton)
	{
	case GRAPHIC_SHADOW:
		pGI->SetShadowLevel(SHADOW_LEVEL::SHADOW_HIGH);
		break;
	case GRAPHIC_SSAO:
		pGI->SetSSAO(true);
		break;
	case GRAPHIC_OUTLINE:
		pGI->SetOutline(true);
		break;
	case GRAPHIC_FILTER:
		pGI->SetLUT((CRenderer::LUT)m_iGraphicState[GRAPHIC_FILTER]);
		break;
	case GRAPHIC_FOG:
		pGI->SetFog(true);
		break;
	}
}

HRESULT CSettingUI::mainRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };
	_float fMainAlpha = 1.f;

	// 메인 백그라운드
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::HALF_BACK_GRAD, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	if (m_eCurType == SETTING_TYPE::SET_GRAPHIC)
		graphicRender();
	else
		audioRender();

	return S_OK;
}

HRESULT CSettingUI::graphicRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };
	_float fMainAlpha = 1.f;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitle.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_TERMINAL_SETTING, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < GRAPHIC_END; ++i)
	{
		// 슬롯
		fAlpha = m_fElemAlpha[2 + i] * 0.2f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicSlot[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(13);
		m_pVIBuffer->Render();

		// 슬롯 텍스트
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicText[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SHADOW + i, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (i == GRAPHIC_FILTER || m_iGraphicState[i] > 0)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnLeftBtn[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnLeft[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::LEFT_ARROW_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
		else
		{
			vColor3 = { 0.5f, 0.5f, 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnLeftBtn[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();

			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnLeft[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::LEFT_ARROW_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}

		if (i == GRAPHIC_FILTER || m_iGraphicState[i] < 1)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnRightBtn[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnRight[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::RIGHT_ARROW_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
		else
		{
			vColor3 = { 0.5f, 0.5f, 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnRightBtn[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();

			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnRight[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::RIGHT_ARROW_GLOW, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}

	}

	for (_uint i = 0; i < GRAPHIC_END; ++i)
	{
		if (i != GRAPHIC_FILTER)
		{
			if (m_iGraphicState[i] == 1)
			{
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ON, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();
			}
			else
			{
				// 버튼 내용 텍스트
				vColor3 = { 0.5f, 0.5f, 0.5f };
				if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_OFF, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(8);
				m_pVIBuffer->Render();
			}
		}
		else
		{
			switch (m_iGraphicState[i])
			{
			case CRenderer::LUT_EXPOSURE_MINUS:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_MINUS, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_EXPOSURE_PLUS:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_PLUS, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_FUJI:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_FUJI, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_GRUNGY:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_GRUNGY, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_SOUTH:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_SOUTH, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_KURO:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_KURO, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_LAST:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_ACHROMATIC, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			case CRenderer::LUT_DEFAULT:
				// 버튼 내용 텍스트
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4 + i], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGraphicBtnText[i].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LUT_DEFAULT, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(10);
				m_pVIBuffer->Render();

				break;
			}
		}


	}

	return S_OK;
}

HRESULT CSettingUI::audioRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };
	_float fMainAlpha = 1.f;

	return S_OK;
}

CSettingUI * CSettingUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSettingUI* pInstance = new CSettingUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSettingUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSettingUI::Clone(void * pArg)
{
	CSettingUI* pInstance = new CSettingUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSettingUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSettingUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
