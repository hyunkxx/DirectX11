#include "stdafx.h"
#include "..\Public\UI_Mouse.h"
#include "GameMode.h"
#include "GameInstance.h"

CUI_Mouse::CUI_Mouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Mouse::CUI_Mouse(const CUI_Mouse& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Mouse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Mouse::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX, m_fY, m_fZ));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Mouse::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	Get_MousePos();
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX + 5.f, m_fY - 6.f, m_fZ));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

}

void CUI_Mouse::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Mouse::Render()
{
	if (true == m_bRender)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(Setup_ShaderResources()))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pVIBuffer->Render();
	}
	return S_OK;
}

void CUI_Mouse::RenderGUI()
{		
}

_float3	CUI_Mouse::Get_MousePos()
{
	POINT		MousePos{};
	
	GetCursorPos(&MousePos);
	ScreenToClient(g_hWnd, &MousePos);

	UINT           pNumViewports;
	m_pContext->RSGetViewports(&pNumViewports, NULL);
	D3D11_VIEWPORT*		pViewPort = new D3D11_VIEWPORT[pNumViewports];
	m_pContext->RSGetViewports(&pNumViewports, pViewPort);

	float Viewport_Width = (float)pViewPort[0].Width;
	float Viewport_Height = (float)pViewPort[0].Height;

	Safe_Delete_Array(pViewPort);

	_float3	fMousePos;
	m_fX = MousePos.x - (Viewport_Width * 0.5f);
	m_fY = -MousePos.y + (Viewport_Height * 0.5f);

	return _float3(m_fX, m_fY, 0.0f);
}


void CUI_Mouse::Set_Texchange(_bool change)
{
	 m_bTexchange = change;
	 if (true == m_bTexchange)
		 m_iTexNum = 1;
	 else
		 m_iTexNum = 0;
}

HRESULT CUI_Mouse::Add_Components()
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UIMOUSE,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;


	return S_OK;

}

HRESULT CUI_Mouse::Setup_ShaderResources()
{
	if (nullptr == m_pShader || nullptr == m_pTexture)
		return E_FAIL;
	if (1 == m_iTexNum)
		m_iTexNum = 1;
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_iTexNum)))
			return E_FAIL;
	
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShader->SetRawValue("g_fColorR", &m_fColorR, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &m_fColorG, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &m_fColorB, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &m_fColorA, sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

CUI_Mouse* CUI_Mouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Mouse* pInstance = new CUI_Mouse(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Mouse";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Mouse::Clone(void* pArg)
{
	CUI_Mouse* pInstance = new CUI_Mouse(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Mouse";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Mouse::Free ()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);
}


