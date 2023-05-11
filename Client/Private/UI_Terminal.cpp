#include "stdafx.h"
#include "..\Public\UI_Terminal.h"
#include "GameInstance.h"
#include "UI_MainScreen.h"
#include "UI_Mouse.h"

CUI_Terminal::CUI_Terminal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Terminal::CUI_Terminal(const CUI_Terminal& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Terminal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Terminal::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Load();

	return S_OK;
}

void CUI_Terminal::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	for (_uint i = 0; i < 51; ++i)
	{
		XMStoreFloat4x4(&(m_DescList[i]->WorldMatrix), XMMatrixScaling(m_DescList[i]->fWidth, m_DescList[i]->fHeight, 1.f) *
			 XMMatrixTranslation(m_DescList[i]->fX, m_DescList[i]->fY, m_DescList[i]->fZ));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}
	for (_uint i = 0; i < 30; ++i)
	{
		//TimeAcc += (_float)TimeDelta;
		XMStoreFloat4x4(&(m_RotDescList[i]->WorldMatrix), XMMatrixScaling(m_RotDescList[i]->fWidth, m_RotDescList[i]->fHeight, 1.f)
			*	XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_RotDescList[i]->fDegree) * TimeAcc*0.1f)
			* XMMatrixTranslation(m_RotDescList[i]->fX, m_RotDescList[i]->fY, m_RotDescList[i]->fZ));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}
		
}

void CUI_Terminal::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Terminal::Render()
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
				m_pVIBuffer->Render();
			}
			++Bufferindex;
		}

		Bufferindex = 0;
		for (auto& pBuffer : m_RotBufferList)
		{
			if (nullptr != pBuffer)
			{
				if (FAILED(Setup_ShaderResourcesRot(Bufferindex)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
			++Bufferindex;
		}
	
	return S_OK;
}

void CUI_Terminal::RenderGUI()
{
	ImGui::Begin("ter ID");
	ImGui::InputInt("ID", &m_iObjectID);
	ImGui::End();
}

void CUI_Terminal::SerectUI()
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
	fMousePos.x = MousePos.x - (Viewport_Width * 0.5f);
	fMousePos.y = -MousePos.y + (Viewport_Height * 0.5f);
	fMousePos.z = 0.f;

	_vector vMousePos = XMLoadFloat3(&fMousePos);

	if (0 != m_DescList.size())
	{
		for (auto& pCutDesc : m_DescList)
		{
			_float Dist = 1.f;
			// 버퍼의 각 꼭지점
			_vector P0 = XMVectorSet(pCutDesc->fX - pCutDesc->fWidth * 0.5f, pCutDesc->fY + pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);
			_vector P1 = XMVectorSet(pCutDesc->fX + pCutDesc->fWidth * 0.5f, pCutDesc->fY + pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);
			_vector P2 = XMVectorSet(pCutDesc->fX + pCutDesc->fWidth * 0.5f, pCutDesc->fY - pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);
			_vector P3 = XMVectorSet(pCutDesc->fX - pCutDesc->fWidth * 0.5f, pCutDesc->fY - pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);

			// UI크기에 맞춰서 범위체크
			if ((XMVectorGetX(P0) < XMVectorGetX(vMousePos)) && (XMVectorGetX(P1) > XMVectorGetX(vMousePos)))
			{
				if ((XMVectorGetY(P0) > XMVectorGetY(vMousePos)) && (XMVectorGetY(P2) < XMVectorGetY(vMousePos)))
				{
					pCutDesc->OnRect = true;
					CGameInstance*		pGameInstance = CGameInstance::GetInstance();
					Safe_AddRef(pGameInstance);
					CGameObject* pMouse = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("UI_Mouse"));
					dynamic_cast<CUI_Mouse*>(pMouse)->Set_Texchange(true);
					Safe_Release(pGameInstance);
				}
				else
				{
					pCutDesc->OnRect = false;

				}
			}
			else
			{
				pCutDesc->OnRect = false;

			}
		}
	}
}





HRESULT CUI_Terminal::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*버퍼 하나당  세이브파일 한개 , 로드할때 파일  하나당 할당 하나. 리스트 원소개수로*/

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UI,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;

}


HRESULT CUI_Terminal::Setup_ShaderResources(_uint Bufferindex)
{
	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[Bufferindex]->iTexNum)))
			return E_FAIL;
	}

	if (nullptr != m_DescList[Bufferindex])
	{
		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[Bufferindex]->WorldMatrix))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[Bufferindex]->fColorR), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[Bufferindex]->fColorG), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[Bufferindex]->fColorB), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[Bufferindex]->fColorA), sizeof(_float))))
			return E_FAIL;
	
		m_iPass = m_DescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_Terminal::Setup_ShaderResourcesRot(_uint Bufferindex)
{
	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_RotDescList[Bufferindex]->iTexNum)))
			return E_FAIL;
	}

	if (nullptr != m_RotDescList[Bufferindex])
	{
			
		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_RotDescList[Bufferindex]->WorldMatrix))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_RotDescList[Bufferindex]->fColorR), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_RotDescList[Bufferindex]->fColorG), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_RotDescList[Bufferindex]->fColorB), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_RotDescList[Bufferindex]->fColorA), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_RotDescList[Bufferindex]->iPass;
	}
	return S_OK;
}


CUI_Terminal* CUI_Terminal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Terminal* pInstance = new CUI_Terminal(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Terminal";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Terminal::Clone(void* pArg)
{
	CUI_Terminal* pInstance = new CUI_Terminal(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Terminal";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Terminal::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);

	for (auto& Buffer : m_BufferList)
	{
		Safe_Release(Buffer);
		Buffer = nullptr;
	}
	m_BufferList.clear();

	for (auto& Desc : m_DescList)
	{
		delete Desc;
		Desc = nullptr;
	}
	m_DescList.clear();


	for (auto& Buffer : m_RotBufferList)
	{
		Safe_Release(Buffer);
		Buffer = nullptr;
	}
	m_RotBufferList.clear();

	for (auto& Desc : m_RotDescList)
	{
		delete Desc;
		Desc = nullptr;
	}
	m_RotDescList.clear();


}

void CUI_Terminal::Load()
{
	for (int i = 0; i < 51; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Terminal%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TERMINALDESC*  Desc = new TERMINALDESC;
		ZeroMemory(Desc, sizeof(TERMINALDESC));
		XMStoreFloat4x4(&Desc->WorldMatrix, XMMatrixIdentity());

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc->fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Duration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->TimeAcc), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Cool), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeXAnimP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeXAnimM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeYAnimP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeYAnimM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bCoolTime), sizeof(_bool), &dwByte, nullptr);
			
			if (0 == dwByte)
				break;
		}

		if (nullptr != Desc)
		{
				m_DescList.push_back(Desc);

				m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
				if (nullptr == m_pVIBuffer)
					return;

				m_BufferList.push_back(m_pVIBuffer);
		}
	}


	for (int i = 0; i < 30; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/RotTerminal%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TERMINALDESC*  Desc = new TERMINALDESC;
		ZeroMemory(Desc, sizeof(TERMINALDESC));
		XMStoreFloat4x4(&Desc->WorldMatrix, XMMatrixIdentity());

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc->fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Duration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->TimeAcc), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Cool), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeXAnimP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeXAnimM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeYAnimP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeYAnimM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bCoolTime), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		if (nullptr != Desc)
		{
			Desc->fDegree = XMConvertToRadians(90.f);
			m_RotDescList.push_back(Desc);

			m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
			if (nullptr == m_pVIBuffer)
				return;

			m_RotBufferList.push_back(m_pVIBuffer);
		}


	}

}

void CUI_Terminal::Rot(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector		vRight, vUp, vLook;
		_matrix WorldMat;
		WorldMat = XMLoadFloat4x4(&(m_RotDescList[indexstart]->WorldMatrix));
		vRight = WorldMat.r[0];
		vUp = WorldMat.r[1];
		vLook = WorldMat.r[2];

		_matrix		RotationMatrix;

		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 
			XMConvertToRadians(m_RotDescList[indexstart]->fDegree));
		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);

		WorldMat.r[0] = vRight;
		WorldMat.r[1] = vUp;
		WorldMat.r[2] = vLook;
		XMStoreFloat4x4(&(m_RotDescList[indexstart]->WorldMatrix), WorldMat);

		
	}
}

void CUI_Terminal::Add_Alpha(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i<indexend; ++i)
	{
		if (0.f > m_DescList[i]->fColorA)
		{
			m_DescList[i]->fColorA += (_float)TimeDelta*400.f;
		}
		if (0.0f < m_DescList[i]->fColorA)
			m_DescList[i]->fColorA = 0.0f;
	}
}

void CUI_Terminal::Minus_Alpha(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i<indexend; ++i)
	{
		if (-255.0f < m_DescList[i]->fColorA)
		{
			m_DescList[i]->fColorA -= (_float)TimeDelta*400.0f;
		}
		if (-255.0f > m_DescList[i]->fColorA)
			m_DescList[i]->fColorA = -255.0f;
	}
}
void CUI_Terminal::Add_ShaderY(_double TimeDelta)
{
	if (1.0f>m_ShaderY)
		m_ShaderY += (_float)TimeDelta;
}

void CUI_Terminal::Minus_ShaderY(_double TimeDelta)
{
	if (0.0f < m_ShaderY)
		m_ShaderY -= (_float)TimeDelta;
}
void CUI_Terminal::Start_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_DescList[i]->OriPos.x, m_DescList[i]->OriPos.y, 0.01f, 1.f);
		_vector PrePos = XMVectorSet(m_DescList[i]->PrePos.x, m_DescList[i]->PrePos.y, 0.01f, 1.f);
		_vector vDir = OriPos - PrePos;
		_vector CurrentPos = XMVectorSet(m_DescList[i]->fX, m_DescList[i]->fY, 0.01f, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta*30.f;
		_vector pos = CurrentPos;
		pos += (OriPos - CurrentPos) * (_float)TimeDelta * 3.f;
		_float Dist = Distance(XMVectorSet(m_DescList[i + 6]->fX, m_DescList[i + 6]->fY, 0.01f, 1.f), CurrentPos);
		if (2.f < Dist)
		{
			m_DescList[i]->fX = XMVectorGetX(pos);
			m_DescList[i]->fY = XMVectorGetY(pos);
		}
	}
}
void CUI_Terminal::End_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_DescList[i]->OriPos.x, m_DescList[i]->OriPos.y, 0.01f, 1.f);
		_vector PrePos = XMVectorSet(m_DescList[i]->PrePos.x, m_DescList[i]->PrePos.y, 0.01f, 1.f);
		_vector vDir = OriPos - PrePos;
		_vector CurrentPos = XMVectorSet(m_DescList[i]->fX, m_DescList[i]->fY, 0.01f, 1.f);
		CurrentPos -= XMVector4Normalize(vDir) * (_float)TimeDelta*30.f;
		_vector pos = CurrentPos;
		pos += (PrePos - CurrentPos) * (_float)TimeDelta;
		m_DescList[i]->fX = XMVectorGetX(pos);
		m_DescList[i]->fY = XMVectorGetY(pos);
	}
}

_float CUI_Terminal::Distance(_fvector TargetPos, _fvector CurrentPos)
{
	return XMVectorGetX(XMVector3Length(TargetPos - CurrentPos));
}

void CUI_Terminal::Middle_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_DescList[i]->OriPos.x, m_DescList[i]->OriPos.y, 0.01f, 1.f);
		_vector PrePos = XMVectorSet(m_DescList[i]->PrePos.x, m_DescList[i]->PrePos.y, 0.01f, 1.f);
		_vector vDir = OriPos - PrePos;
		_vector CurrentPos = XMVectorSet(m_DescList[i]->fX, m_DescList[i]->fY, 0.01f, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta*100.f;
		_vector pos = CurrentPos;
		pos += (OriPos - CurrentPos) * (_float)TimeDelta;
		m_DescList[i]->fX = XMVectorGetX(pos);
		m_DescList[i]->fY = XMVectorGetY(pos);
	}

}