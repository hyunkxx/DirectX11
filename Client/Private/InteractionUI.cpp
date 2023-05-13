#include "stdafx.h"
#include "..\Public\InteractionUI.h"

#include "GameMode.h"
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

	m_KeyDesc.fX = 790.f;
	m_KeyDesc.fY = 410.f;
	m_KeyDesc.fWidth = 28.f;
	m_KeyDesc.fHeight = 36.f;

	m_InteractionBack.fX = 940.f;
	m_InteractionBack.fY = 410.f;
	m_InteractionBack.fWidth = 256.f;
	m_InteractionBack.fHeight = 32.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	XMStoreFloat4x4(&m_KeyDesc.WorldMatrix, XMMatrixScaling(m_KeyDesc.fWidth, m_KeyDesc.fHeight, 1.f) * XMMatrixTranslation(m_KeyDesc.fX - g_iWinSizeX * 0.5f, -m_KeyDesc.fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_InteractionBack.WorldMatrix, XMMatrixScaling(m_InteractionBack.fWidth, m_InteractionBack.fHeight, 1.f) * XMMatrixTranslation(m_InteractionBack.fX - g_iWinSizeX * 0.5f, -m_InteractionBack.fY + g_iWinSizeY * 0.5f, 0.f));

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

	if (pGameInstance->InputKey(DIK_T) == KEY_STATE::TAP)
	{
		if(m_bActive)
			SetRender(false);
		else
			SetRender(true);
	}

	if (m_UIRender)
	{
		m_fAlpha += (_float)TimeDelta * 2.f;
		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;

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
		m_fAlpha -= (_float)TimeDelta * 2.f;
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			m_bActive = false;
			m_UIRender = false;
			m_fCurrentSpriteIndex = 0.f;
		}
	}


}

void CInteractionUI::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::LateTick(TimeDelta);

	if(m_bActive)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInteractionUI::Render()
{
	// F Button
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_KeyDesc.WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pKeyTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	// Interaction Back
	if (m_UIRender)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_InteractionBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pListBackTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pVIBuffer->Render();

		// Sprite
		ORTHO_DESC orthoDesc;
		orthoDesc = m_InteractionBack;
		orthoDesc.fWidth = m_InteractionBack.fWidth + 8.f;
		orthoDesc.fHeight = m_InteractionBack.fHeight + 8.f;
		XMStoreFloat4x4(&orthoDesc.WorldMatrix, XMMatrixScaling(orthoDesc.fWidth, orthoDesc.fHeight, 1.f) * XMMatrixTranslation(orthoDesc.fX - g_iWinSizeX * 0.5f, -orthoDesc.fY + g_iWinSizeY * 0.5f, 0.f));

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &orthoDesc.WorldMatrix)))
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

	}

	return S_OK;
}

void CInteractionUI::RenderGUI()
{
	ImGui::Begin("On Button");

	if (m_bActive)
		ImGui::Text("Mouse In");
	else
		ImGui::Text("Mouse Out");

	ImGui::End();
}

void CInteractionUI::SetRender(_bool bValue)
{
	if (bValue)
	{
		m_bActive = bValue;
		m_UIRender = bValue;
	}
	else
		m_UIRender = bValue;
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

}
