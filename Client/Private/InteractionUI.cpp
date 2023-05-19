#include "stdafx.h"
#include "..\Public\InteractionUI.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

CInteractionUI::CInteractionUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteractionUI::CInteractionUI(const CInteractionUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteractionUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractionUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	m_SpriteSize = { 4.f, 4.f };

	m_KeyDesc.fX = 780.f;
	m_KeyDesc.fY = 410.f;
	m_KeyDesc.fWidth = 28.f;
	m_KeyDesc.fHeight = 36.f;

	m_InteractionBack.fX = 980.f;
	m_InteractionBack.fY = 410.f;
	m_InteractionBack.fWidth = 332.f;
	m_InteractionBack.fHeight = 42.f;

	m_Text.fX = 980.f;
	m_Text.fY = 412.f;
	m_Text.fWidth = 256.f;
	m_Text.fHeight = 32.f;

	m_Sprite = m_InteractionBack;
	m_Sprite.fX += 3.f;
	m_Sprite.fWidth = m_InteractionBack.fWidth + 12.f;
	m_Sprite.fHeight = m_InteractionBack.fHeight + 12.f;

	m_InterIcon = m_KeyDesc;
	m_InterIcon.fX += 56.f;
	m_InterIcon.fWidth = 32.f;
	m_InterIcon.fHeight = 32.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	XMStoreFloat4x4(&m_KeyDesc.WorldMatrix, XMMatrixScaling(m_KeyDesc.fWidth, m_KeyDesc.fHeight, 1.f) * XMMatrixTranslation(m_KeyDesc.fX - g_iWinSizeX * 0.5f, -m_KeyDesc.fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_InteractionBack.WorldMatrix, XMMatrixScaling(m_InteractionBack.fWidth, m_InteractionBack.fHeight, 1.f) * XMMatrixTranslation(m_InteractionBack.fX - g_iWinSizeX * 0.5f, -m_InteractionBack.fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_Text.WorldMatrix, XMMatrixScaling(m_Text.fWidth, m_Text.fHeight, 1.f) * XMMatrixTranslation(m_Text.fX - g_iWinSizeX * 0.5f, -m_Text.fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_Sprite.WorldMatrix, XMMatrixScaling(m_Sprite.fWidth, m_Sprite.fHeight, 1.f) * XMMatrixTranslation(m_Sprite.fX - g_iWinSizeX * 0.5f, -m_Sprite.fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_InterIcon.WorldMatrix, XMMatrixScaling(m_InterIcon.fWidth, m_InterIcon.fHeight, 1.f) * XMMatrixTranslation(m_InterIcon.fX - g_iWinSizeX * 0.5f, -m_InterIcon.fY + g_iWinSizeY * 0.5f, 0.f));

	m_GageBarBack.fX = g_iWinSizeX >> 1;
	m_GageBarBack.fY = g_iWinSizeY >> 1;
	m_GageBarBack.fWidth = 300.f;
	m_GageBarBack.fHeight = 50.f;
	CAppManager::ComputeOrtho(&m_GageBarBack);

	return S_OK;
}

void CInteractionUI::Start()
{
}

void CInteractionUI::Tick(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

	//if (pGameMode->OnMouse(m_KeyDesc))
	//	m_bActive = true;
	//else
	//	m_bActive = false;

	if (m_bUIActive)
	{
		m_fUIAlpha += (_float)TimeDelta * 2.f;
		if (m_fUIAlpha >= 1.f)
			m_fUIAlpha = 1.f;

		// Sprite
		m_fSpriteAcc += (_float)TimeDelta;
		if (m_fSpriteAcc >= m_fSpriteSpeed)
		{
			m_fSpriteAcc = 0.f;
			if (m_fCurrentSpriteIndex < (m_SpriteSize.x * m_SpriteSize.y) - 1)
				m_fCurrentSpriteIndex += 1.f;
			else
				m_fCurrentSpriteIndex = (m_SpriteSize.x * m_SpriteSize.y) - 1.f;
		}
	}
	else
	{
		m_fUIAlpha -= (_float)TimeDelta * 2.f;
		if (m_fUIAlpha <= 0.f)
		{
			m_bUIRender = false;
			m_fUIAlpha = 0.f;
			m_fCurrentSpriteIndex = 0.f;
		}
	}

	if (m_bFButtonActive)
	{
		m_fFButtonAlpha += (_float)TimeDelta * 2.f;
		if (m_fFButtonAlpha >= 1.f)
			m_fFButtonAlpha = 1.f;
	}
	else
	{
		m_fFButtonAlpha -= (_float)TimeDelta * 2.f;
		if (m_fFButtonAlpha <= 0.f)
		{
			m_fFButtonAlpha = 0.f;
			m_bFButtonRender = false;
		}
	}

	if (m_fGageBar > 1.f)
		m_fGageBar = 0.f;

}

void CInteractionUI::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::LateTick(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInteractionUI::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// F Button
	if (m_bFButtonRender)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_KeyDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fFButtonAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pKeyTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(5);
		m_pVIBuffer->Render();
	}

	// Interaction Back
	if (m_bUIRender)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_InteractionBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pListBackTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pVIBuffer->Render();

		// Sprite
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_Sprite.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_SpriteXY", &m_SpriteSize, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_CurrentCount", &m_fCurrentSpriteIndex, sizeof(_float))))
			return E_FAIL;

		_float3 vColor = { 1.f, 1.f, 0.5f };
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pSpriteTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(6);
		m_pVIBuffer->Render();

		// Icon
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fUIAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_InterIcon.WorldMatrix)))
			return E_FAIL;
		switch (m_eInterType)
		{
		case INTER_ACTIVATE:
			if (FAILED(m_pInterTypeTexture[INTER_ACTIVATE]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case INTER_INSPECT:
			if (FAILED(m_pInterTypeTexture[INTER_INSPECT]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case INTER_SIMPLE_CHEST:
		case INTER_STANDARD_CHEST:
		case INTER_EXPANDED_CHEST:
			if (FAILED(m_pInterTypeTexture[INTER_SIMPLE_CHEST]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		default:
			MSG_BOX("Interaction Type Unknown : CInteractionUI::Render()");
			break;
		}

		m_pShader->Begin(5);
		m_pVIBuffer->Render();

		// Text
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fUIAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_Text.WorldMatrix)))
			return E_FAIL;

		switch (m_eInterType)
		{
		case INTER_ACTIVATE:
			if (FAILED(m_pTextTexture[INTER_ACTIVATE]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case INTER_INSPECT:
			if (FAILED(m_pTextTexture[INTER_INSPECT]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case INTER_SIMPLE_CHEST:
			if (FAILED(m_pTextTexture[INTER_SIMPLE_CHEST]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case INTER_STANDARD_CHEST:
			if (FAILED(m_pTextTexture[INTER_STANDARD_CHEST]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		case INTER_EXPANDED_CHEST:
			if (FAILED(m_pTextTexture[INTER_EXPANDED_CHEST]->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		default:
			MSG_BOX("Interaction Type Unknown : CInteractionUI::Render()");
			break;
		}

		m_pShader->Begin(5);
		m_pVIBuffer->Render();

	}

	if (m_fGageBar > 0.f)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_GageBarBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fGageBar, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWLINE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(2);
		m_pVIBuffer->Render();

	}

	return S_OK;
}

void CInteractionUI::RenderGUI()
{
}

void CInteractionUI::SetInteractionActive(INTERACT_TYPE eType, _bool bValue)
{
	m_eInterType = eType;

	if (bValue)
	{
		m_bUIRender = m_bUIActive = bValue;
		m_bFButtonRender = m_bFButtonActive = bValue;
	}
	else
	{
		m_bUIActive = bValue;
		m_bFButtonActive = bValue;
	}
}

HRESULT CInteractionUI::addComponents()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_KEY_BTN_F,
		TEXT("com_texture_keybtn"), (CComponent**)&m_pKeyTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_LISTBACK,
		TEXT("com_texture_listback"), (CComponent**)&m_pListBackTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_LISTGARD_SPRITE,
		TEXT("com_texture_listgard"), (CComponent**)&m_pSpriteTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_ICON_ACTIVATE,
		TEXT("com_texture_icon_0"), (CComponent**)&m_pInterTypeTexture[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_ICON_INSPECT,
		TEXT("com_texture_icon_1"), (CComponent**)&m_pInterTypeTexture[1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_ICON_CHEST,
		TEXT("com_texture_icon_2"), (CComponent**)&m_pInterTypeTexture[2])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_ACTIVATE_TEXT,
		TEXT("com_texture_text_0"), (CComponent**)&m_pTextTexture[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_INSPECT_TEXT,
		TEXT("com_texture_text_1"), (CComponent**)&m_pTextTexture[1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_SIMPLE_CHEST_TEXT,
		TEXT("com_texture_text_2"), (CComponent**)&m_pTextTexture[2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_STANDARD_CHEST_TEXT,
		TEXT("com_texture_text_3"), (CComponent**)&m_pTextTexture[3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UI_EXPANDED_CHEST_TEXT,
		TEXT("com_texture_text_4"), (CComponent**)&m_pTextTexture[4])))
		return E_FAIL;

	return S_OK;
}

CInteractionUI * CInteractionUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInteractionUI*	pInstance = new CInteractionUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteractionUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteractionUI::Clone(void * pArg)
{
	CInteractionUI*	pInstance = new CInteractionUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteractionUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteractionUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pKeyTexture);
	Safe_Release(m_pListBackTexture);
	Safe_Release(m_pSpriteTexture);

	Safe_Release(m_pInterTypeTexture[0]);
	Safe_Release(m_pInterTypeTexture[1]);
	Safe_Release(m_pInterTypeTexture[2]);

	Safe_Release(m_pTextTexture[0]);
	Safe_Release(m_pTextTexture[1]);
	Safe_Release(m_pTextTexture[2]);
	Safe_Release(m_pTextTexture[3]);
	Safe_Release(m_pTextTexture[4]);
}
