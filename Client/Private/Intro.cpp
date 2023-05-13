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
	for (_uint i = 0; i <(_uint)rhs.m_BufferList.size(); ++i)
	{
		m_BufferList.push_back(rhs.m_BufferList[i]);
		Safe_AddRef(rhs.m_BufferList[i]);
		m_DescList.push_back(rhs.m_DescList[i]);
	}
}

HRESULT CIntro::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Load();

	return S_OK;
}

HRESULT CIntro::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pGI = CGameInstance::GetInstance();
	pGI->PlaySoundEx(L"Intro_BGM.mp3", SOUND_CHANNEL::BGM, VOLUME_BGM);

	

	return S_OK;
}

void CIntro::Tick(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	if (false == m_bLoadingEnd)
	{
		LoadingBar(3, TimeDelta);
	}
	if (true == m_bLoadingEnd)
	{
		m_DescList[2]->fColorA = 0.f;
		m_DescList[3]->fColorA = 0.f;

		m_DescList[1]->fColorA += (_float)TimeDelta * 180.f;

		if (255.f < m_DescList[1]->fColorA)
		{
			m_DescList[1]->fColorA = 255.f;

			m_DescList[4]->bSizeXAnimM = true;
			m_DescList[4]->bSizeYAnimM = true;
		}

		pGI->BGMSmoothOff(TimeDelta * 0.05f);
	}


	if (true == m_bXYM)
	{
		for (_uint i = 4; i < 10; ++i)
		{
			m_DescList[i]->bAlphaM = true;
		}
	}

	for (_uint i = 0; i < 10; ++i)
	{
		if ((m_DescList[i]->bSizeYAnimP == true) && (m_DescList[i]->bSizeXAnimP == true))
		{
			SizeXYM(i, TimeDelta);
		}
		else if (m_DescList[i]->bSizeXAnimP == true)
		{
			SizeXAnimP(i, TimeDelta);
		}
		else if (m_DescList[i]->bSizeYAnimP == true)
		{
			SizeYAnimP(i, TimeDelta);
		}

		if ((m_DescList[i]->bSizeXAnimM == true) && (m_DescList[i]->bSizeYAnimM == true))
		{
			SizeXYM(i, TimeDelta);
		}
		else if (m_DescList[i]->bSizeXAnimM == true)
		{
			SizeXAnimM(i, TimeDelta);
		}
		else if (m_DescList[i]->bSizeYAnimM == true)
		{
			SizeYAnimM(i, TimeDelta);
		}

		if (m_DescList[i]->bAlphaM == true)
			AlphaM(i, TimeDelta);
		if (m_DescList[i]->bAlphaP == true)
			AlphaP(i, TimeDelta);
	}

	for (auto& Desc : m_DescList)
	{
		XMStoreFloat4x4(&(Desc->WorldMatrix), XMMatrixScaling(Desc->fWidth, Desc->fHeight, 1.f)
			* XMMatrixTranslation(Desc->fX, Desc->fY, Desc->fZ));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}

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

	_uint Bufferindex = 0;
	for (auto& pBuffer : m_BufferList)
	{
		if (nullptr != pBuffer)
		{
			if (FAILED(Setup_ShaderResources(Bufferindex)))
				return E_FAIL;
			m_pShader->Begin(m_iPass);


			if ((2 == Bufferindex) || (3 == Bufferindex))
			{
				if (false == m_bLoadingEnd)
				{
					pBuffer->Render();
				}
			}
			else
				pBuffer->Render();

			++Bufferindex;
		}
	}
	return S_OK;
}

void CIntro::RenderGUI()
{
	//ImGui::Begin("Loading ID");
	//ImGui::InputInt("ID", &m_iObjectID);

	//ImGui::End();


}



void CIntro::LoadingBar(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_bLoadingEnd = true;
	}

	m_fLoadingBar += (_float)TimeDelta * 180.f;
	m_fLoading = m_fLoadingBar / m_DescList[iNum]->fWidth;

}



void CIntro::SizeXAnimM(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeXAnimM = false;
	}


	_float fWidth = m_DescList[iNum]->fWidth - m_DescList[iNum]->fSpeedWidth * (_float)TimeDelta;
	m_DescList[iNum]->fWidth = m_DescList[iNum]->WorldMatrix._11 = fWidth;
	m_DescList[iNum]->WorldMatrix._22 = m_DescList[iNum]->fHeight;

}

void CIntro::SizeXAnimP(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeXAnimP = false;
	}


	_float fWidth = m_DescList[iNum]->fWidth + m_DescList[iNum]->fSpeedWidth * (_float)TimeDelta;
	m_DescList[iNum]->fWidth = m_DescList[iNum]->WorldMatrix._11 = fWidth;
	m_DescList[iNum]->WorldMatrix._22 = m_DescList[iNum]->fHeight;

}



void CIntro::SizeYAnimM(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeYAnimM = false;
	}


	_float fHeight = m_DescList[iNum]->fHeight - m_DescList[iNum]->fSpeedHeight * (_float)TimeDelta;
	m_DescList[iNum]->WorldMatrix._11 = m_DescList[iNum]->fWidth;
	m_DescList[iNum]->fHeight = m_DescList[iNum]->WorldMatrix._22 = fHeight;

}

void CIntro::SizeYAnimP(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeYAnimP = false;
	}

	_float fWidth = m_DescList[iNum]->fWidth + m_DescList[iNum]->fSpeedWidth * (_float)TimeDelta;
	m_DescList[iNum]->fWidth = m_DescList[iNum]->WorldMatrix._11 = fWidth;

	_float fHeight = m_DescList[iNum]->fHeight + m_DescList[iNum]->fSpeedHeight * (_float)TimeDelta;
	m_DescList[iNum]->fHeight = m_DescList[iNum]->WorldMatrix._22 = fHeight;

}


void CIntro::SizeXYP(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	_float fWidth = m_DescList[iNum]->fWidth + m_DescList[iNum]->fSpeedWidth * (_float)TimeDelta;
	m_DescList[iNum]->fWidth = m_DescList[iNum]->WorldMatrix._11 = fWidth;

	_float fHeight = m_DescList[iNum]->fHeight + m_DescList[iNum]->fSpeedHeight * (_float)TimeDelta;
	m_DescList[iNum]->fHeight = m_DescList[iNum]->WorldMatrix._22 = fHeight;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_bXYM = true;
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeXAnimP = false;
		m_DescList[iNum]->bSizeYAnimP = false;
	}
}



void CIntro::SizeXYM(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	_float fWidth = m_DescList[iNum]->fWidth - m_DescList[iNum]->fSpeedWidth * (_float)TimeDelta;
	m_DescList[iNum]->fWidth = m_DescList[iNum]->WorldMatrix._11 = fWidth;

	_float fHeight = m_DescList[iNum]->fHeight - m_DescList[iNum]->fSpeedHeight * (_float)TimeDelta;
	m_DescList[iNum]->fHeight = m_DescList[iNum]->WorldMatrix._22 = fHeight;


	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_bXYM = true;
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeXAnimM = false;
		m_DescList[iNum]->bSizeYAnimM = false;
	}
}

void CIntro::AlphaM(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	m_DescList[iNum]->fColorA -= m_DescList[iNum]->fSpeedAlpha * (_float)TimeDelta;


	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeYAnimM = false;
	}
}

void CIntro::AlphaP(_uint iNum, _double TimeDelta)
{
	m_DescList[iNum]->TimeAcc += (_float)TimeDelta;

	if (m_DescList[iNum]->TimeAcc > m_DescList[iNum]->Duration)
	{
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeYAnimM = false;
	}

	m_DescList[iNum]->fColorA += m_DescList[iNum]->fSpeedAlpha * (_float)TimeDelta;

}

void CIntro::Reset()
{
	for (_uint iNum = 0; iNum < 9; ++iNum)
	{

		m_DescList[iNum]->Duration = 0.f;
		m_DescList[iNum]->TimeAcc = 0.f;
		m_DescList[iNum]->bSizeXAnimP = false;
		m_DescList[iNum]->bSizeXAnimM = false;
		m_DescList[iNum]->bSizeYAnimP = false;
		m_DescList[iNum]->bSizeYAnimM = false;
		m_DescList[iNum]->bAlphaM = false;
		m_DescList[iNum]->bAlphaP = false;
		m_DescList[iNum]->fSpeedfX = 0.f;
		m_DescList[iNum]->fSpeedfY = 0.f;
		m_DescList[iNum]->fSpeedfZ = 0.f;
		m_DescList[iNum]->fSpeedWidth = 0.f;
		m_DescList[iNum]->fSpeedHeight = 0.f;
		m_DescList[iNum]->fSpeedAlpha = 0.f;
		m_DescList[iNum]->fColorR = 0.f;
		m_DescList[iNum]->fColorG = 0.f;
		m_DescList[iNum]->fColorB = 0.f;
		m_DescList[iNum]->fColorA = 0.f;
	}
}

HRESULT CIntro::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::UILOADING,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;

}

HRESULT CIntro::Setup_ShaderResources(_uint i)
{
	if (nullptr == m_pShader || nullptr == m_pTexture)
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[i]->iTexNum)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &m_DescList[i]->WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShader->SetRawValue("g_fColorR", &m_DescList[i]->fColorR, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &m_DescList[i]->fColorG, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &m_DescList[i]->fColorB, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &m_DescList[i]->fColorA, sizeof(_float))))
		return E_FAIL;
	if (2 == m_DescList[i]->iTexNum)
	{
		if (FAILED(m_pShader->SetRawValue("g_fLoading", &m_fLoading, sizeof(_float))))
			return E_FAIL;

	}
	m_iPass = m_DescList[i]->iPass;
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

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	m_pVIBuffer = nullptr; // 먼저 null 만들어야 리스트에서 해제할때안터짐
	Safe_Release(m_pVIBuffer);
	
	
		for (_uint i = 0; i < m_BufferList.size(); ++i)
		{
			if (nullptr != m_BufferList[i])
			{
				Safe_Release(m_BufferList[i]);
			}
		}
		m_BufferList.clear();
		
	if (!m_bClone)
	{
		for (_uint i = 0; i<m_DescList.size(); ++i)
		{
			if (nullptr != m_DescList[i])
			{
				delete  m_DescList[i];
				m_DescList[i] = nullptr;
			}
		}

		
		m_DescList.clear();
	}
}

void CIntro::Load()
{
	_uint index = 10;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[125] = L"";
		wsprintf(szFileName, L"../../Data/UI/Loading%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		_ulong dwByte = 0;

		LOADINGDESC*  Desc = new LOADINGDESC;
		ZeroMemory(Desc, sizeof(LOADINGDESC));
		XMStoreFloat4x4(&Desc->WorldMatrix, XMMatrixIdentity());

		while (true)
		{
			ReadFile(hFile, &(Desc->fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Duration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Cool), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);


			if (0 == dwByte)
				break;
		}
		CloseHandle(hFile);

		if (nullptr != Desc)
		{
			m_DescList.push_back(Desc);
			
			m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
			if (nullptr == m_pVIBuffer)
				return;

			m_BufferList.push_back(m_pVIBuffer);
		}
	}

}


void CIntro::Save()
{
	_uint index = 0;
	for (auto& Desc : m_DescList)
	{
		TCHAR	szFileName[125] = L"";
		wsprintf(szFileName, L"../../Data/Loading%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		_ulong dwByte = 0;

		WriteFile(hFile, &(Desc->fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->Duration), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->Cool), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fSpeedfX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fSpeedfY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fSpeedfZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fSpeedWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fSpeedHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);

		CloseHandle(hFile);
		++index;
	}
}

