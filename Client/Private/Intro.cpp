#include "stdafx.h"
#include "..\Public\Intro.h"

#include "GameInstance.h"

CIntro::CIntro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CIntro::CIntro(const CIntro& rhs)
	: CGameObject(rhs)
{
}

HRESULT CIntro::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIntro::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = g_iWinSizeX;
	m_fHeight = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CIntro::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CIntro::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CIntro::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

void CIntro::RenderGUI()
{
}

HRESULT CIntro::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::INTRO_BACKGROUND,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIntro::Setup_ShaderResources()
{
	if (nullptr == m_pShader || nullptr == m_pTexture)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CIntro* CIntro::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIntro* pInstance = new CIntro(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CIntro";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIntro::Clone(void* pArg)
{
	CIntro* pInstance = new CIntro(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CIntro";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIntro::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
