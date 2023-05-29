#include "stdafx.h"
#include "..\Public\UI_Tip.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "RenderTarget.h"

CUI_Tip::CUI_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Tip::CUI_Tip(const CUI_Tip& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Tip::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Tip::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Load();
	return S_OK;
}

void CUI_Tip::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGame = CGameInstance::GetInstance();
	
		if ((true == m_bUIRender)&&(AddAlpha(TimeDelta)))
		{
			m_bUIRender = false;
		}

		if (false == m_bUIRender)
		{
			m_TimeAcc += (_float)TimeDelta;
		}
	
		if (8.f < m_TimeAcc)
		{
			if (MinusAlpha(TimeDelta))
			{
				m_TimeAcc = 0.f;
				m_bUIRender = true;
				SetState(DISABLE);
			}
		}
		UVWave(TimeDelta);
}

_bool CUI_Tip::AddAlpha(_double TimeDelta)
{
	for (_uint i = 0; i < (_int)m_DescList.size(); ++i)
	{
		if (0.f > m_DescList[i].fColorA)
		{
			m_DescList[i].fColorA += (_float)TimeDelta * 100.f;
		}
		else
		{
			return true;
		}
	}
	return false;
}

_bool CUI_Tip::MinusAlpha(_double TimeDelta)
{
	for (_uint i = 0; i < (_int)m_DescList.size(); ++i)
	{
		if (-255.f < m_DescList[i].fColorA)
		{
			m_DescList[i].fColorA -= (_float)TimeDelta * 100.f;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void CUI_Tip::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Tip::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	for (_uint i =0; i <(_uint)m_DescList.size(); ++i)
	{
		if (FAILED(Setup_ShaderResources(i)))
			return E_FAIL;
		m_pShader->Begin(m_iPass);
		m_pVIBuffer->Render();
	}
	
	return S_OK;
}

void CUI_Tip::RenderGUI()
{
		
}


HRESULT CUI_Tip::Add_Components()
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UITIP,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_Tip::Setup_ShaderResources(_uint index)
{
	
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Title", m_MainMaskNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_TitleUV", &m_fUV, sizeof(_float2))))
		return E_FAIL;

		XMStoreFloat4x4(&(m_DescList[index].WorldMatrix), XMMatrixScaling(m_DescList[index].fWidth, m_DescList[index].fHeight, 1.f)
			* XMMatrixTranslation(m_DescList[index].fX, m_DescList[index].fY, m_DescList[index].fZ));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index].WorldMatrix))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index].fColorR), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index].fColorG), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index].fColorB), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index].fColorA), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_DescList[index].iPass;
	
	return S_OK;
}


void CUI_Tip::UVWave(_double TimeDelta)
{
	m_fUV.x -= (_float)TimeDelta;

}


void CUI_Tip::Save()
{
	_uint index = 0;
	for (auto& Desc : m_DescList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Tip%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}


}



void CUI_Tip::Load()
{
	_uint index = 5;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Tip%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TIPDESC Desc;
		ZeroMemory(&Desc, sizeof(TIPDESC));

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);

			if (0 == dwByte)
				break;
		}
		m_DescList.push_back(Desc);
		CloseHandle(hFile);

	}

}



CUI_Tip* CUI_Tip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Tip* pInstance = new CUI_Tip(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Tip";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Tip::Clone(void* pArg)
{
	CUI_Tip* pInstance = new CUI_Tip(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Tip";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Tip::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);


}


