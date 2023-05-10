#include "stdafx.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = g_iWinSizeX;
	m_fHeight = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fTextWidth = 400.f;
	m_fTextHeight = 20.f;
	m_fTextX = g_iWinSizeX >> 1;
	m_fTextY = g_iWinSizeY - 40.f;

	m_fGageWidth = 108.f;
	m_fGageHeight = 280.f;
	m_fGageX = g_iWinSizeX >> 1;
	m_fGageY = g_iWinSizeY >> 1;

	m_fSideWidth = 1280.f;
	m_fSideHeight = 200;
	m_fSideX = g_iWinSizeX >> 1;
	m_fSideY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_TextWorldMatrix, XMMatrixScaling(m_fTextWidth, m_fTextHeight, 1.f) * XMMatrixTranslation(m_fTextX - g_iWinSizeX * 0.5f, -m_fTextY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_GageWorldMatrix, XMMatrixScaling(m_fGageWidth, m_fGageHeight, 1.f) * XMMatrixTranslation(m_fGageX - g_iWinSizeX * 0.5f, -m_fGageY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_SideWorldMatrix, XMMatrixScaling(m_fSideWidth, m_fSideHeight, 1.f) * XMMatrixTranslation(m_fSideX - g_iWinSizeX * 0.5f, -m_fSideY + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CBackGround::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	ComputeLoadRatioLerp(TimeDelta);

}

void CBackGround::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_BackgroundResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(Setup_TextResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(Setup_GageResources(GAGE_BACK)))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();
	
	if (FAILED(Setup_GageResources(GAGE_FRONT)))
		return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	if (FAILED(Setup_SideResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

void CBackGround::RenderGUI()
{
}

HRESULT CBackGround::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::BLACK,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::LOADING_TEXT,
		TEXT("com_texture_text"), (CComponent**)&m_pTextTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::LOADING_SIDE,
		TEXT("com_texture_side"), (CComponent**)&m_pSideTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::LOADING_BACK,
		TEXT("com_texture_back"), (CComponent**)&m_pBackTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::LOADING_FRONT,
		TEXT("com_texture_front"), (CComponent**)&m_pFrontTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Setup_BackgroundResources()
{
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

HRESULT CBackGround::Setup_TextResources()
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_TextWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Setup_GageResources(_uint nGageType)
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_GageWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	switch (nGageType)
	{
	case GAGE_BACK:
		if (FAILED(m_pBackTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case GAGE_FRONT:
		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fCurLoadAcc, sizeof(float))))
			return E_FAIL;
		if (FAILED(m_pFrontTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CBackGround::Setup_SideResources()
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_SideWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pSideTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

void CBackGround::ComputeLoadRatioLerp(_double TimeDelta)
{
	_vector vCurLoadAcc = XMVectorSet(m_fCurLoadAcc, 0.f, 0.f, 0.f);
	_vector vLoadRatio = XMVectorSet(m_fLoadRatio, 0.f, 0.f, 0.f);

	m_fCurLoadAcc = XMVectorGetX(XMVectorLerp(vCurLoadAcc, vLoadRatio, (_float)TimeDelta));
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CBackGround";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CBackGround";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pTextTexture);
	Safe_Release(m_pBackTexture);
	Safe_Release(m_pFrontTexture);
	Safe_Release(m_pSideTexture);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
