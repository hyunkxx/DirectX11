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
	
#pragma endregion

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));

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
}

void CSettingUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}
}

void CSettingUI::keyInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

}

HRESULT CSettingUI::mainRender()
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
