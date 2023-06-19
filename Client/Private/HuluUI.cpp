#include "stdafx.h"
#include "..\Public\HuluUI.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "AppManager.h"

#include "Inventory.h"
#include "PlayerState.h"
#include "EchoSystem.h"
#include "CameraMovement.h"

#include "ItemDB.h"
#include "TerminalUI.h"

CHuluUI::CHuluUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CHuluUI::CHuluUI(const CHuluUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHuluUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHuluUI::Initialize(void * pArg)
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

	m_OrthoHuru.fWidth = 620.f;
	m_OrthoHuru.fHeight = 508.f;
	m_OrthoHuru.fX = g_iWinSizeX - 310.f;
	m_OrthoHuru.fY = 454.f;
	CAppManager::ComputeOrtho(&m_OrthoHuru);

	// HuruTitle
	m_OrthoTitleText.fWidth = 512.f * 0.65f;
	m_OrthoTitleText.fHeight = 64.f * 0.65f;
	m_OrthoTitleText.fX = 330.f;
	m_OrthoTitleText.fY = 120.f;
	CAppManager::ComputeOrtho(&m_OrthoTitleText);

	m_OrthoTitleSub.fWidth = 512.f * 0.65f;
	m_OrthoTitleSub.fHeight = 64.f * 0.65f;
	m_OrthoTitleSub.fX = 330.f;
	m_OrthoTitleSub.fY = 170.f;
	CAppManager::ComputeOrtho(&m_OrthoTitleSub);

	m_OrthoActiveGage.fWidth = 512.f * 0.65f;
	m_OrthoActiveGage.fHeight = 20.f;
	m_OrthoActiveGage.fX = 330.f;
	m_OrthoActiveGage.fY = 200.f;
	CAppManager::ComputeOrtho(&m_OrthoActiveGage);

	m_OrthoActiveText.fWidth = 512.f * 0.65f;
	m_OrthoActiveText.fHeight = 64.f * 0.65f;
	m_OrthoActiveText.fX = 330.f;
	m_OrthoActiveText.fY = 230.f;
	CAppManager::ComputeOrtho(&m_OrthoActiveText);

	// Icons
	m_OrthoMainSlot.fWidth = 90.f;
	m_OrthoMainSlot.fHeight = 90.f;
	m_OrthoMainSlot.fX = 100.f;
	m_OrthoMainSlot.fY = 180.f;
	CAppManager::ComputeOrtho(&m_OrthoMainSlot);
	m_OrthoMainIcon.fWidth = 40.f;
	m_OrthoMainIcon.fHeight = 40.f;
	m_OrthoMainIcon.fX = 100.f;
	m_OrthoMainIcon.fY = 180.f;
	CAppManager::ComputeOrtho(&m_OrthoMainIcon);

	m_OrthoHuluIcon.fWidth = 160.f;
	m_OrthoHuluIcon.fHeight = 160.f;
	m_OrthoHuluIcon.fX = 320.f;
	m_OrthoHuluIcon.fY = 240.f;
	CAppManager::ComputeOrtho(&m_OrthoHuluIcon);

	for (_uint i = 0; i < 5; ++i)
	{
		m_OrthoActiveCount[i].fWidth = 16.f * 0.7f;
		m_OrthoActiveCount[i].fHeight = 25.f * 0.7f;
		m_OrthoActiveCount[i].fX = 430.f + (i * (15.f * 0.7f));
		m_OrthoActiveCount[i].fY = 200.f;
		CAppManager::ComputeOrtho(&m_OrthoActiveCount[i]);
	}

	for (_uint i = 0; i < 4; ++i)
	{
		m_OrthoText[i].fWidth = 512.f * 0.6f;
		m_OrthoText[i].fHeight = 64.f * 0.6f;
		m_OrthoText[i].fX = 320.f;
		m_OrthoText[i].fY = 380.f + (i * (64.f * 0.6f));
		CAppManager::ComputeOrtho(&m_OrthoText[i]);

		m_OrthoBack[i].fWidth = 400.f;
		m_OrthoBack[i].fHeight = 64.f * 0.6f;
		m_OrthoBack[i].fX = 320.f;
		m_OrthoBack[i].fY = 380.f + (i * (64.f * 0.6f));
		CAppManager::ComputeOrtho(&m_OrthoBack[i]);
	}

	m_OrthoAbsorb.fWidth = 512.f * 0.6f;
	m_OrthoAbsorb.fHeight = 64.f * 0.6f;
	m_OrthoAbsorb.fX = 320.f;
	m_OrthoAbsorb.fY = m_OrthoBack[0].fY - (64.f * 0.6f);
	CAppManager::ComputeOrtho(&m_OrthoAbsorb);

#pragma endregion

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));

	return S_OK;
}

void CHuluUI::Start()
{
}

void CHuluUI::PreTick(_double TimeDelta)
{
	CGameObject::PreTick(TimeDelta);
}

void CHuluUI::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_bActive)
	{
		//keyInput(TimeDelta);
		elemAlphaUpdate(TimeDelta);
	}
}

void CHuluUI::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);

	if(m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHuluUI::Render()
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

HRESULT CHuluUI::RenderShadow()
{
	return S_OK;
}

void CHuluUI::RenderGUI()
{
}

void CHuluUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_Open_2.wem.wav", SOUND_UI_FEEDBACK, VOLUME_VFX);
}

void CHuluUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();
}

void CHuluUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CHuluUI::addComponents()
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

void CHuluUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;

	elemAlphaReset(0);
	m_bElemAlphaStart[0] = true;
}

void CHuluUI::stateDisable()
{
	m_bRender = false;
}

void CHuluUI::elemAlphaUpdate(_double TimeDelta)
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

void CHuluUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}
}

HRESULT CHuluUI::mainRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };

	_float fMainAlpha = 1.f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fMainAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_DRAGON_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 타이틀
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitleText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_TERMINALDATA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitleSub.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_TERMINALLEVEL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	vColor3 = { 0.3f, 0.3f, 0.3f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveGage.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	_uint iActiveCount = m_pEchoSystem->GetActiveEchoCount();
	string strCount = to_string(iActiveCount);
	_float fActiveRatio = (_float)iActiveCount / 12.f;
	_uint iDigit = (_uint)strCount.size();

	if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &fActiveRatio, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveGage.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(2);
	m_pVIBuffer->Render();

	if (iDigit == 1)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveCount[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCount[0] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}
	else
	{
		for (_uint i = 0; i < iDigit; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveCount[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strCount[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
	}

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveCount[2].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SLASH, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveCount[3].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_1, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveCount[4].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_2, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	vColor3 = { 0.5f, 0.5f, 0.5f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActiveText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_TERMINALSUB, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// 버튼
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMainSlot.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMainIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_ICON_TERMINAL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoHuru.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_HURU, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	vColor3 = SLOT_LEGEND_COLOR;
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAbsorb.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ABSORBCLASS, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < 4; ++i)
	{
		fAlpha = m_fElemAlpha[5 + i] * 0.3f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBack[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(13);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7 + i], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoText[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TEXT_TERMINAL100 + i, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

CHuluUI * CHuluUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHuluUI* pInstance = new CHuluUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHuluUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHuluUI::Clone(void * pArg)
{
	CHuluUI* pInstance = new CHuluUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHuluUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHuluUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
